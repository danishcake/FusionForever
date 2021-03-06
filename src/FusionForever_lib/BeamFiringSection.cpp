#include "StdAfx.h"
#include "BeamFiringSection.h"
#include "Ricochet.h"

BeamFiringSection::BeamFiringSection(void)
: Section()
{
	beam_ = NULL;
	ltv_firing_ = false;
	beam_sum_time_ = 0;
	beam_charge_time_ = 0.5f;
	beam_cooldown_time_ = 0.5f;
	beam_fire_time_ = 2.0f;
	beam_deco_spawn_ = 0;
	beam_charge_.SetPosition(Vector3f(0, 4.0f, 0));
	beam_energy_use_ = 5;
	charge_up_sound_ = "";
	firing_sound_ = "";
	//beam_sound_ = LoopingBuffer("fire1.wav");
}

BeamFiringSection::~BeamFiringSection(void)
{
	delete beam_;
}

void BeamFiringSection::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	beam_->SetFirer_ID(root_->GetSectionID());

	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	if(firing_&& PowerRequirement(10))
	{
		if(!beam_sound_.GetPlaying())
			beam_sound_.Start();
		beam_sum_time_ += _timespan;
		if(!ltv_firing_ && charge_up_sound_.length() != 0)
			SoundManager::Instance().PlaySample(charge_up_sound_);

		if(beam_sum_time_ < beam_charge_time_)
		{
			beam_charge_.SetScale(beam_sum_time_ / beam_charge_time_);
			PowerTick(-beam_energy_use_ * _timespan);
		}
		if(beam_sum_time_ >= beam_charge_time_ && beam_sum_time_ < beam_charge_time_ + beam_fire_time_)
		{
			beam_->Tick(_timespan, _spawn_dec, ltv_transform_, _enemies);
			beam_charge_.SetScale(Random::RandomRange(0.9f, 1.1f));
			PowerTick(-beam_energy_use_ * _timespan);
		}
		if(beam_sum_time_ > beam_charge_time_ + beam_fire_time_ &&
		   beam_sum_time_ < beam_charge_time_ + beam_fire_time_ + beam_cooldown_time_)
		{
			beam_charge_.SetScale( 1.0f  - ((beam_sum_time_ - beam_charge_time_ - beam_fire_time_) / beam_charge_time_));
			PowerTick(-beam_energy_use_ * _timespan);
		}
		if(beam_sum_time_ >= beam_charge_time_ + beam_fire_time_ + beam_cooldown_time_)
		{
			beam_sum_time_ = 0;
			beam_charge_.SetScale(0);
		}
		
	} else
	{
		if(beam_sound_.GetPlaying())
			beam_sound_.Stop();
		if(ltv_firing_ && beam_sum_time_ > beam_cooldown_time_)
			beam_sum_time_ = beam_cooldown_time_;
		if(beam_sum_time_ > 0)
			beam_sum_time_-= _timespan;
		beam_charge_.SetScale(beam_sum_time_ / beam_cooldown_time_);
	}
	ltv_firing_ = firing_;
	beam_charge_.Tick(_timespan, ltv_transform_, _spawn_dec);
}

void BeamFiringSection::DrawSelf()
{
	Section::DrawSelf();
	//Now draw beam if firing
	if(firing_ && health_ > 0)
	{
		if(beam_sum_time_ >= beam_charge_time_ && beam_sum_time_ < beam_charge_time_ + beam_fire_time_)
			beam_->DrawSelf();
	}
	if(health_ > 0)
		beam_charge_.DrawSelf();
}


void BeamFiringSection::RegisterMetadata()
{
	Section::RegisterMetadata();
	SectionMetadata::RegisterSectionTag(section_type_, "Beam");
	SectionMetadata::RegisterSectionTag(section_type_, "Weapon");
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", beam_->GetMaxDistance());
	SectionMetadata::RegisterSectionKeyValue(section_type_, "BeamDPS", beam_->GetDPS());
}
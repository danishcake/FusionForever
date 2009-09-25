#include "StdAfx.h"
#include "SectionDecoration.h"
#include "Section.h"
#include "Core.h"
#include "Puff.h"
#include "Explosion.h"

void SectionDecoration::InitialiseGraphics()
{
	//Do nothing
}

SectionDecoration::SectionDecoration(Section* _section)
{
	//assert(_section->GetSubsections().size() == 0);
	//assert(_section->GetParent() == NULL);
	assert(_section);

	section_ = _section;
	position_ = _section->GetGlobalPosition();
	angle_ = _section->GetGlobalAngle();
	velocity_ = _section->GetRoot()->GetVelocity() + 
				Vector3f(Random::RandomRange(-10, 10), Random::RandomRange(-50, 50), 0);
	spin_ = Random::RandomRange(-360, 360);
	if(section_->GetMass() < 50)
		lifetime_ = 0.25f;
	else if(section_->GetMass() < 1000)
	{
		lifetime_ = Random::RandomRange(0.5f, 0.5f + 4.5f * (section_->GetMass() / 1000.0f));
	} else
		lifetime_ = Random::RandomRange(0.5f, 5.0f);
		
	full_lifetime_ = lifetime_;
	explosion_timer_ = 0.2f;
	big_explosion_timer_ = Random::RandomRange(lifetime_ * 0.8f, full_lifetime_ * 2.0f);
	original_color_ = section_->GetColor();
}

SectionDecoration::~SectionDecoration()
{
	delete section_;
}
void SectionDecoration::Tick(float _timespan, Matrix4f _transform, std::vector<Decoration_ptr>& _decoration_spawn)
{
	float speed = this->velocity_.length();
	speed = speed * expf(-_timespan * 1);
	velocity_.normalize();
	velocity_ *= speed;

	Decoration::Tick(_timespan, _transform, _decoration_spawn);
	section_->SetPosition(position_);
	section_->SetAngle(angle_);
	section_->DeathTick();
	GLColor faded = original_color_;
	faded.a = static_cast<unsigned char>(255.0f * (lifetime_/ full_lifetime_));
	section_->SetColor(faded);
	section_->SetOutlineColor(GLColor(255,255,255, faded.a));

	explosion_timer_ -= _timespan;
	if(explosion_timer_ <= 0)
	{
		for(int i = 0; i < 3; i++)
		{
			Decoration_ptr puff = new Puff();
			puff->SetPosition(GetGlobalPosition() + Vector3d(Random::RandomCentered(0, section_->GetRadius()), Random::RandomCentered(0, section_->GetRadius()), 0));
			_decoration_spawn.push_back(puff);
		}
		explosion_timer_ = Random::RandomRange(0.01f, 0.25f);
	}

	big_explosion_timer_ -= _timespan;
	if(big_explosion_timer_ <= 0)
	{
		for(int i = 0; i < 8; i++)
		{
			Decoration_ptr explosion = new Explosion();
			explosion->SetPosition(GetGlobalPosition() + Vector3d(Random::RandomCentered(0, section_->GetRadius()), Random::RandomCentered(0, section_->GetRadius()), 0));
			_decoration_spawn.push_back(explosion);
		}
		big_explosion_timer_ = full_lifetime_ * 10; //Never explode again
	}


}

void SectionDecoration::DrawSelf()
{
	section_->DrawSelf();
}
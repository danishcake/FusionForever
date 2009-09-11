#include "StdAfx.h"
#include "HeavyChainGun.h"
#include "Sparks.h"
#include "PlasmaRound.h"

//Initialise all static class members
bool HeavyChainGun::initialised_ = false;
int HeavyChainGun::outline_dl_ = 0;
int HeavyChainGun::outline_verts_index_ = 0;
int HeavyChainGun::fill_dl_ = 0;
int HeavyChainGun::fill_verts_index_ = 0;

float HeavyChainGun::minimum_fire_time_ = 0.04f;
float HeavyChainGun::start_fire_time_ = 0.25f;
float HeavyChainGun::spin_up_time_ = 2.0f;

HeavyChainGun::HeavyChainGun(void)
: FiringSection()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	//Get the cached vertices
	outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_.SetDisplayList(outline_dl_);
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	findRadius();

	health_ = FlexFloat(750, 750);
	cooldown_time_ = 0.15f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 200;
	spun_up_factor_ = 0.0f;
	firing_time_ = 0;
	section_type_ = "HeavyChainGun";
}

HeavyChainGun::~HeavyChainGun(void)
{
}

void HeavyChainGun::InitialiseGraphics()
{
	//Initialise outline
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0.0f, -3.0f, 0.0f));	//0
	temp_outline->push_back(Vector3f(3.0f, 0.0f,  0.0f));	//1
	temp_outline->push_back(Vector3f(1.5f, 8.0f,  0.0f));	//2
	temp_outline->push_back(Vector3f(-1.5f, 8.0f, 0.0f));   //3
	temp_outline->push_back(Vector3f(-3.0f, 0,    0.0f));	//4

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[2]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[3]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[3]);
	temp_fill->push_back((*temp_outline)[4]);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void HeavyChainGun::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	
	if(firing_)
	{
		spun_up_factor_ += _timespan / spin_up_time_;
		firing_time_ += _timespan;
		if(spun_up_factor_ > 1.0f)
			spun_up_factor_ = 1.0f;

		if(cooldown_ <= 0.0f && PowerRequirement(8))
		{
			fire_projectile(new PlasmaRound(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(1, 7), 0)), _spawn_prj);
			cooldown_ = start_fire_time_ - (start_fire_time_ - minimum_fire_time_) * spun_up_factor_;
			PowerTick(-2);
		}
	} else
	{
		spun_up_factor_ -= _timespan / spin_up_time_;
		firing_time_ = 0;
		if(spun_up_factor_ < 0)
			spun_up_factor_ = 0;
	}
	ltv_firing_time_ = firing_time_;
}

void HeavyChainGun::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 1800);
	SectionMetadata::RegisterSectionTag(section_type_, "FiringRamps");
}
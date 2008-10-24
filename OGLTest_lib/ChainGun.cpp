#include "StdAfx.h"
#include "ChainGun.h"
#include "Sparks.h"

//Initialise all static class members
bool ChainGun::initialised_ = false;
int ChainGun::outline_dl_ = 0;
int ChainGun::outline_verts_index_ = 0;
int ChainGun::fill_dl_ = 0;
int ChainGun::fill_verts_index_ = 0;

float ChainGun::minimum_fire_time_ = 0.04f;
float ChainGun::start_fire_time_ = 0.2f;
float ChainGun::spin_up_time_ = 1.5f;
float ChainGun::phase_two_time_ = 10;

ChainGun::ChainGun(void)
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

	health_ = FlexFloat(500, 500);
	cooldown_time_ = 0.1f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 150;
	spun_up_factor_ = 0.0f;
	firing_time_ = 0;
}

ChainGun::~ChainGun(void)
{
}

void ChainGun::InitialiseGraphics()
{
	//Initialise outline
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0, -2.5f, 0));	//0
	temp_outline->push_back(Vector3f(2.5f, 0, 0));	//1
	temp_outline->push_back(Vector3f(1, 7, 0));		//2
	temp_outline->push_back(Vector3f(-1, 7, 0));    //3
	temp_outline->push_back(Vector3f(-2.5f, 0, 0));	//4

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

void ChainGun::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	
	if(firing_)
	{
		spun_up_factor_ += _timespan / spin_up_time_;
		firing_time_ += _timespan;
		if(spun_up_factor_ > 1.0f)
			spun_up_factor_ = 1.0f;

		if(cooldown_ <= 0.0f && PowerRequirement(5))
		{
			if(firing_time_ < phase_two_time_)
			{
				fire_projectile(new SmallBullet(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(1, 7), 0)), _spawn_prj);
				cooldown_ = start_fire_time_ - (start_fire_time_ - minimum_fire_time_) * spun_up_factor_;
			}
			else
			{
				fire_projectile(new SmallBullet(Vector3f(Random::RandomRange(-2, 0), Random::RandomRange(1, 7), 0)), _spawn_prj);
				fire_projectile(new SmallBullet(Vector3f(Random::RandomRange(2, 0), Random::RandomRange(1, 7), 0)), _spawn_prj);
				cooldown_ = (start_fire_time_ - (start_fire_time_ - minimum_fire_time_) * spun_up_factor_) * 1.2f;
			}
			
			PowerTick(-1);
		}
	} else
	{
		spun_up_factor_ -= _timespan / spin_up_time_;
		firing_time_ = 0;
		if(spun_up_factor_ < 0)
			spun_up_factor_ = 0;
	}
	ltv_firing_time_ = firing_time_;

	//After phase two kicks in make some decorations
	if(firing_time_ > phase_two_time_ && ltv_firing_time_ <= phase_two_time_)
	{
		_spawn_dec.push_back(new Sparks());
	}
}

void ChainGun::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("SectionType", "ChainGun");
}
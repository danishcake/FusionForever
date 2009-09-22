#include "StdAfx.h"
#include "SmartBomber.h"
#include "Core.h"
#include "HomingBomb.h"

//Initialise all static class members
bool SmartBomber::initialised_ = false;
int SmartBomber::outline_dl_ = 0;
int SmartBomber::outline_verts_index_ = 0;
int SmartBomber::fill_dl_ = 0;
int SmartBomber::fill_verts_index_ = 0;

SmartBomber::SmartBomber(void)
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

	health_ = FlexFloat(400, 400);
	cooldown_time_ = 3.0f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 400;
	section_type_ = "SmartBomber";
}

SmartBomber::~SmartBomber(void)
{
}

void SmartBomber::InitialiseGraphics(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0, -2.5f, 0));	//0
	temp_outline->push_back(Vector3f(2.5f, 0, 0));	//1
	temp_outline->push_back(Vector3f(1, 5, 0));		//2
	temp_outline->push_back(Vector3f(-1, 5, 0));    //3
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

void SmartBomber::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(35))
		{
			BaseEntity* target = NULL;
			Core* core_target = root_->GetTarget();
			if(core_target)
			{
				target = core_target;
			}
			else if(_enemies.size() > 0)
			{
				int index = Random::RandomIndex(static_cast<int>(_enemies.size()));
				target = _enemies[index];
			}
			HomingBomb* bomb = new HomingBomb(Vector3f(0, 3, 0), target);

			fire_projectile(bomb, _spawn_prj);
			cooldown_ = cooldown_time_;
			PowerTick(25);
		}
	}
}

void SmartBomber::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 1000);
	SectionMetadata::RegisterSectionTag(section_type_, "Mine");
}
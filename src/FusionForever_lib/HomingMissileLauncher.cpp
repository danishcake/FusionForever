#include "StdAfx.h"
#include "HomingMissileLauncher.h"
#include "HomingMissile.h"
#include "Core.h"

//Initialise all static class members
bool HomingMissileLauncher::initialised_ = false;
int HomingMissileLauncher::outline_dl_ = 0;
int HomingMissileLauncher::outline_verts_index_ = 0;
int HomingMissileLauncher::fill_dl_ = 0;
int HomingMissileLauncher::fill_verts_index_ = 0;

HomingMissileLauncher::HomingMissileLauncher(void)
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

	health_ = FlexFloat(1000, 1000);
	cooldown_time_ = 0.9f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 300;
	section_type_ = "HomingMissileLauncher";

}

HomingMissileLauncher::~HomingMissileLauncher(void)
{
}

void HomingMissileLauncher::InitialiseGraphics(void)
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

void HomingMissileLauncher::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(25))
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
			HomingMissile* hm = new HomingMissile(Vector3f(0, 5, 0), target, fill_.GetFillColor());

			fire_projectile(hm, _spawn_prj);
			cooldown_ = cooldown_time_;
			PowerTick(-5);
		}
	}
}

void HomingMissileLauncher::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 1315);
	SectionMetadata::RegisterSectionTag(section_type_, "Homing");
}
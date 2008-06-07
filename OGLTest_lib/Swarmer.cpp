#include "StdAfx.h"
#include "Swarmer.h"
#include "SwarmMissile.h"

//Initialise all static class members
bool Swarmer::initialised_ = false;
int Swarmer::outline_dl_ = 0;
int Swarmer::outline_verts_index_ = 0;
int Swarmer::fill_dl_ = 0;
int Swarmer::fill_verts_index_ = 0;

Swarmer::Swarmer(void)
: FiringSection()
{
	if(!initialised_)
	{
		initialise_outline();
		initialise_fill();
		initialised_ = true;
	}
   //Get the cached vertices
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = outline_dl_;
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = fill_dl_;
	findRadius();

	health_ = 600;
	max_health_ = health_;
	cooldown_time_ = 0.15f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
}

Swarmer::~Swarmer(void)
{
}

void Swarmer::initialise_fill(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = Datastore::Instance().GetVerts(outline_verts_index_);

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
	fill_dl_ = CreateFillDisplayList(temp_fill);
}

void Swarmer::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0, -2.5f, 0));	//0
	temp_outline->push_back(Vector3f(2.5f, 0, 0));	//1
	temp_outline->push_back(Vector3f(1, 5, 0));		//2
	temp_outline->push_back(Vector3f(-1, 5, 0));    //3
	temp_outline->push_back(Vector3f(-2.5f, 0, 0));	//4

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);
}

void Swarmer::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f)
		{
			BaseEntity* target = NULL;
			if(_enemies.size() > 0)
			{
				int index = Random::RandomIndex(static_cast<int>(_enemies.size()));
				target = (BaseEntity*)(_enemies[index]);
			}
			SwarmMissile* hm = new SwarmMissile(Vector3f(0, 5, 0), target);

			fire_projectile(hm, _spawn_prj);
			cooldown_ = cooldown_time_;
		}
	}
}

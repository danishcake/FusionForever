#include "StdAfx.h"
#include "HomingMissileLauncher.h"
#include "HomingMissile.h"
#include "SmallBullet.h"

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

	health_ = 1000;
	max_health_ = health_;
	cooldown_time_ = 0.4f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
}

HomingMissileLauncher::~HomingMissileLauncher(void)
{
}

void HomingMissileLauncher::initialise_fill(void)
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

void HomingMissileLauncher::initialise_outline(void)
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

void HomingMissileLauncher::Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f)
		{
			HomingMissile* hm = new HomingMissile(Vector3f(0, 5, 0));
			if(_enemies.size() > 0)
			{
				int index = rand() % static_cast<int>(_enemies.size());
				Section* target = (Section*)(*_enemies.begin());
				HomingJoin* hj = new HomingJoin(hm, target);
				hm->RegisterHomingJoin(hj);
				target->RegisterHomingJoin(hj);
			}
			fire_projectile(hm, _spawn_prj);
			cooldown_ = cooldown_time_;
		}
	}
}
#include "StdAfx.h"
#include "Blaster.h"

bool Blaster::initialised_ = false;
int Blaster::outline_dl_ = 0;
int Blaster::outline_verts_index_ = 0;
int Blaster::fill_dl_ = 0;
int Blaster::fill_verts_index_ = 0;

Blaster::Blaster(void)
: FiringSection()
{
	if(!initialised_)
	{
		Blaster::initialise_outline();
		Blaster::initialise_fill();
		initialised_ = true;
	}
	outline_verts_ = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_display_list_ = outline_dl_;
	fill_verts_ = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_display_list_ = fill_dl_;
	findRadius();

	health_ = 500;
	cooldown_time_ = 0.1f;
}

Blaster::~Blaster(void)
{
}

void Blaster::initialise_fill(void)
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

void Blaster::initialise_outline(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0, -2.5f, 0));	//0
	temp_outline->push_back(Vector3f(2.5f, 0, 0));	//1
	temp_outline->push_back(Vector3f(1, 5, 0));		//2
	temp_outline->push_back(Vector3f(-1, 5, 0));//3
	temp_outline->push_back(Vector3f(-2.5f, 0, 0));		//4

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = CreateOutlinedDisplayList(temp_outline);
}

void Blaster::Tick(float _timespan, std::list<Projectile_ptr>& _spawn_prj, std::list<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::list<Core_ptr>& _enemies)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec,_transform, _enemies);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f)
		{
			fire_projectile(new SmallBullet(Vector3f(0, 5, 0)), _spawn_prj);
			cooldown_ = cooldown_time_;
		}
	}
}
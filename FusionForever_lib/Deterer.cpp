#include "StdAfx.h"
#include "Deterer.h"
#include "MiniBolt.h"

//Initialise all static class members
bool Deterer::initialised_ = false;
int Deterer::outline_dl_ = 0;
int Deterer::outline_verts_index_ = 0;
int Deterer::fill_dl_ = 0;
int Deterer::fill_verts_index_ = 0;

Deterer::Deterer(void)
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

	health_ = FlexFloat(1500, 1500);
	cooldown_time_ = 1.25f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 450;
	section_type_ = "Deterer";
}

Deterer::~Deterer(void)
{
	
}

void Deterer::InitialiseGraphics()
{
	//Initialise outline
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(0, -3, 0));	//0
	temp_outline->push_back(Vector3f(3, 0, 0));	//1
	temp_outline->push_back(Vector3f(0, 3, 0));		//2
	temp_outline->push_back(Vector3f(-3, 0, 0));    //3

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[1]);
	temp_fill->push_back((*temp_outline)[2]);

	temp_fill->push_back((*temp_outline)[0]);
	temp_fill->push_back((*temp_outline)[2]);
	temp_fill->push_back((*temp_outline)[3]);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void Deterer::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(50))
		{
			Projectile_ptr p1 = new MiniBolt(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(-2, 2), 0));
			p1->SetVelocity(Vector3f(1, 0, 0) * 200);
			Projectile_ptr p2 = new MiniBolt(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(-2, 2), 0));
			p2->SetVelocity(Vector3f(1, 1, 0) * 200);
			Projectile_ptr p3 = new MiniBolt(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(-2, 2), 0));
			p3->SetVelocity(Vector3f(0, 1, 0) * 200);
			Projectile_ptr p4 = new MiniBolt(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(-2, 2), 0));
			p4->SetVelocity(Vector3f(-1, 1, 0) * 200);
			Projectile_ptr p5 = new MiniBolt(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(-2, 2), 0));
			p5->SetVelocity(Vector3f(-1, 0, 0) * 200);
			Projectile_ptr p6 = new MiniBolt(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(-2, 2), 0));
			p6->SetVelocity(Vector3f(-1, -1, 0) * 200);
			Projectile_ptr p7 = new MiniBolt(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(-2, 2), 0));
			p7->SetVelocity(Vector3f(0, -1, 0) * 200);
			Projectile_ptr p8 = new MiniBolt(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(-2, 2), 0));
			p8->SetVelocity(Vector3f(1, -1, 0) * 200);

			fire_projectile(p1, _spawn_prj);
			fire_projectile(p2, _spawn_prj);
			fire_projectile(p3, _spawn_prj);
			fire_projectile(p4, _spawn_prj);
			fire_projectile(p5, _spawn_prj);
			fire_projectile(p6, _spawn_prj);
			fire_projectile(p7, _spawn_prj);
			fire_projectile(p8, _spawn_prj);

			cooldown_ = cooldown_time_;

			PowerTick(-35);
		}
	}
}

#include "StdAfx.h"
#include "FlakCannon.h"
#include "Flak.h"

//Initialise all static class members
bool FlakCannon::initialised_ = false;
int FlakCannon::outline_dl_ = 0;
int FlakCannon::outline_verts_index_ = 0;
int FlakCannon::fill_dl_ = 0;
int FlakCannon::fill_verts_index_ = 0;

FlakCannon::FlakCannon(void)
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
	cooldown_time_ = 1.5f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 450;
}

FlakCannon::~FlakCannon(void)
{
	
}

void FlakCannon::InitialiseGraphics()
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

void FlakCannon::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(50))
		{
			int flecette_count = Random::RandomRange(10, 15);
			for(int i = 0; i < flecette_count; i++)
			{
				Projectile_ptr proj = new Flak(Vector3f(Random::RandomRange(-6, 6), Random::RandomRange(-2, 2), 0));
				proj->SetVelocity(Vector3f(Random::RandomRange(-45, 45), 450, 0));
				fire_projectile(proj, _spawn_prj);
			}
			cooldown_ = cooldown_time_;

			PowerTick(-20);
		}
	}
}

void FlakCannon::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("SectionType", "FlakCannon");
}

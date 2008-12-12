#include "StdAfx.h"
#include "Blaster.h"
#include "SmallBullet.h"

//Initialise all static class members
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
	cooldown_time_ = 0.133f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 150;
}

Blaster::~Blaster(void)
{
}

void Blaster::InitialiseGraphics()
{
	//Initialise outline
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

void Blaster::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(5))
		{
			fire_projectile(new SmallBullet(Vector3f(Random::RandomRange(-2, 2), Random::RandomRange(1, 7), 0)), _spawn_prj);
			cooldown_ = cooldown_time_;
			PowerTick(-1);
		}
	}
}

void Blaster::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("SectionType", "Blaster");
}

static Section_ptr CreateInstance()
{
	return new Blaster();
}

static ListAdder l = ListAdder(CreateInstance, "Blaster");
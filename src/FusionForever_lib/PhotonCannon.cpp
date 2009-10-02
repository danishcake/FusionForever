#include "StdAfx.h"
#include "PhotonCannon.h"
#include "PhotonPulse.h"
#include "Triangulate.h"

//Initialise all static class members
bool PhotonCannon::initialised_ = false;
int PhotonCannon::outline_dl_ = 0;
int PhotonCannon::outline_verts_index_ = 0;
int PhotonCannon::fill_dl_ = 0;
int PhotonCannon::fill_verts_index_ = 0;

PhotonCannon::PhotonCannon(void)
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

	health_ = FlexFloat(600, 600);
	cooldown_time_ = 0.10f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 300;
	section_type_ = "PhotonCannon";
}

PhotonCannon::~PhotonCannon(void)
{
	
}

void PhotonCannon::InitialiseGraphics()
{
	//Initialise outline
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	//M 0,11 L -2,7 L -3,3 L -2,-1 L 0,-2 L 2,-1 L 3,3 L 2,7 L 0,11 z
	temp_outline->push_back(Vector3f( 0, 11, 0));
	temp_outline->push_back(Vector3f(-2,  7, 0));
	temp_outline->push_back(Vector3f(-3,  3, 0)); 
	temp_outline->push_back(Vector3f(-2, -1, 0)); 
	temp_outline->push_back(Vector3f( 0, -2, 0)); 
	temp_outline->push_back(Vector3f( 2, -1, 0)); 
	temp_outline->push_back(Vector3f( 3,  3, 0)); 
	temp_outline->push_back(Vector3f( 2,  7, 0)); 

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	Triangulate::Process(temp_outline, temp_fill);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void PhotonCannon::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(10))
		{
			Projectile_ptr p1 = new PhotonPulse(Vector3f(0, 2, 0));
			fire_projectile(p1, _spawn_prj);
			cooldown_ = cooldown_time_;

			PowerTick(-5);
		}
	}
}

void PhotonCannon::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	//Todo this might be wrong atm
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 2250);
}
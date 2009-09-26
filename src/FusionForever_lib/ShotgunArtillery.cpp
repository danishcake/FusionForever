#include "StdAfx.h"
#include "ShotgunArtillery.h"
#include "ShotgunShell.h"
#include "Triangulate.h"

//Initialise all static class members
bool ShotgunArtillery::initialised_ = false;
int ShotgunArtillery::outline_dl_ = 0;
int ShotgunArtillery::outline_verts_index_ = 0;
int ShotgunArtillery::fill_dl_ = 0;
int ShotgunArtillery::fill_verts_index_ = 0;

ShotgunArtillery::ShotgunArtillery(void)
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

	health_ = FlexFloat(700, 700);
	cooldown_time_ = 2.00f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 300;
	section_type_ = "ShotgunArtillery";
}

ShotgunArtillery::~ShotgunArtillery(void)
{
	
}

void ShotgunArtillery::InitialiseGraphics()
{
	//Initialise outline
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	//M 1,3 L 3,4 L 3,1 L 2,0 L 2,-2 L 4,-2 L 3,-5 L -3,-5 L -4,-2 L -2,-2 L -2,0 L -3,1 L -3,4 L -1,3 L -1,1 L 1,1 L 1,3 z
	temp_outline->push_back(Vector3f( 1,  3, 0));
	temp_outline->push_back(Vector3f( 3,  4, 0));
	temp_outline->push_back(Vector3f( 3,  1, 0)); 
	temp_outline->push_back(Vector3f( 2,  0, 0)); 
	temp_outline->push_back(Vector3f( 2, -2, 0)); 
	temp_outline->push_back(Vector3f( 4, -2, 0)); 
	temp_outline->push_back(Vector3f( 3, -5, 0));
	temp_outline->push_back(Vector3f(-3, -5, 0));
	temp_outline->push_back(Vector3f(-4, -2, 0)); 
	temp_outline->push_back(Vector3f(-2, -2, 0)); 
	temp_outline->push_back(Vector3f(-2,  0, 0)); 
	temp_outline->push_back(Vector3f(-3,  1, 0)); 
	temp_outline->push_back(Vector3f(-3,  4, 0)); 
	temp_outline->push_back(Vector3f(-1,  3, 0)); 
	temp_outline->push_back(Vector3f(-1,  1, 0)); 
	temp_outline->push_back(Vector3f( 1,  1, 0)); 

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	Triangulate::Process(temp_outline, temp_fill);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void ShotgunArtillery::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(10))
		{
			Projectile_ptr p1 = new ShotgunShell(Vector3f(0, 2, 0), 0);
			fire_projectile(p1, _spawn_prj);
			cooldown_ = cooldown_time_;

			PowerTick(-8);
		}
	}
}

void ShotgunArtillery::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	//Todo this might be wrong atm
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 1920);
}
#include "StdAfx.h"
#include "HeavyBlaster.h"
#include "MediumBullet.h"
#include "SVGParser.h"
#include "Triangulate.h"

//Initialise all static class members
bool HeavyBlaster::initialised_ = false;
int HeavyBlaster::outline_dl_ = 0;
int HeavyBlaster::outline_verts_index_ = 0;
int HeavyBlaster::fill_dl_ = 0;
int HeavyBlaster::fill_verts_index_ = 0;

HeavyBlaster::HeavyBlaster(void)
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
	cooldown_time_ = 0.15f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 300;
	section_type_ = "HeavyBlaster";
}

HeavyBlaster::~HeavyBlaster(void)
{

}

void HeavyBlaster::InitialiseGraphics()
{
	//Initialise outline and fill from embedded SVG from Inkscape
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	*temp_outline = SVGParser::ParsePath("M 0,-2.5 L 2.5,0 L 2.5,2.5 L 2,4.5 L 1,4.5 L 0,2 L -1,4.5 L -2,4.5 L -2.5,2.5 L -2.5,0 L 0,-2.5 z");
	temp_outline->pop_back();

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	Triangulate::Process(temp_outline, temp_fill);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void HeavyBlaster::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(15))
		{
			Projectile_ptr p1 = new MediumBullet(Vector3f(0, 4, 0));
			fire_projectile(p1, _spawn_prj);
			cooldown_ = cooldown_time_;

			PowerTick(-4);
			SoundManager::Instance().PlaySample("Fire10.wav");
		}
	}
}

void HeavyBlaster::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	//Todo this might be wrong atm
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 1920);
}
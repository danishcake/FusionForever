#include "StdAfx.h"
#include "LightDestroyer.h"
#include "MediumBullet.h"
#include "Ricochet.h"
#include "SVGParser.h"
#include "Triangulate.h"

//Initialise all static class members
bool LightDestroyer::initialised_ = false;
int LightDestroyer::outline_dl_ = 0;
int LightDestroyer::outline_verts_index_ = 0;
int LightDestroyer::fill_dl_ = 0;
int LightDestroyer::fill_verts_index_ = 0;

LightDestroyer::LightDestroyer(void)
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

	health_ = FlexFloat(650, 650);
	cooldown_time_ = 0.4f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 350;
	section_type_ = "LightDestroyer";
}

LightDestroyer::~LightDestroyer(void)
{

}

void LightDestroyer::InitialiseGraphics()
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

void LightDestroyer::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_ && cooldown_ <= 0.0f && PowerRequirement(50) && !firing_lock_)
	{
		PowerTick(-25);
		firing_lock_ = true;
		fire_count_ = 3;
		SoundManager::Instance().PlaySample("Fire17.wav");
	}
	if(firing_lock_ && fire_count_ > 0 && cooldown_ <= 0.0f)
	{
		MediumBullet* p1 = new MediumBullet(Vector3f(0, 10, 0));
		MediumBullet* p2 = new MediumBullet(Vector3f(5, 5, 0));
		MediumBullet* p3 = new MediumBullet(Vector3f(-5, 5, 0));
		fire_projectile(p1, _spawn_prj);
		fire_projectile(p2, _spawn_prj);
		fire_projectile(p3, _spawn_prj);

		Ricochet* d1 = new Ricochet(GetGlobalAngle() - 30, false);
		d1->SetPosition(GetGlobalPosition());
		Ricochet* d2 = new Ricochet(GetGlobalAngle() + 30, false);
		d2->SetPosition(GetGlobalPosition());
		_spawn_dec.push_back(d1);
		_spawn_dec.push_back(d2);


		cooldown_ = 0.09f;
		fire_count_--;
		
	}
	if(firing_lock_ && fire_count_ == 0)
	{
		cooldown_ = cooldown_time_;
		firing_lock_ = false;
	}
}

void LightDestroyer::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	//Todo this might be wrong atm
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 1920);
}
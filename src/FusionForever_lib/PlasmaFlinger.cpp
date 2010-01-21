#include "StdAfx.h"
#include "PlasmaFlinger.h"
#include "PlasmaSeeker.h"
#include "Core.h"
#include "SVGParser.h"
#include "Triangulate.h"

//Initialise all static class members
bool PlasmaFlinger::initialised_ = false;
int PlasmaFlinger::outline_dl_ = 0;
int PlasmaFlinger::outline_verts_index_ = 0;
int PlasmaFlinger::fill_dl_ = 0;
int PlasmaFlinger::fill_verts_index_ = 0;

PlasmaFlinger::PlasmaFlinger(void)
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
	cooldown_time_ = 1.8f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 400;
	section_type_ = "PlasmaFlinger";

}

PlasmaFlinger::~PlasmaFlinger(void)
{
}

void PlasmaFlinger::InitialiseGraphics(void)
{
	//Initialise outline and fill from embedded SVG from Inkscape
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	*temp_outline = SVGParser::ParsePath("M 0,3 L 1,4 L 2,3 L 3,4 L 4,3 L 3,0 L 3,-1 L 2,-2 L -2,-2 L -3,-1 L -3,0 L -4,3 L -3,4 L -2,3 L -1,4 L 0,3 z");
	temp_outline->pop_back();

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	Triangulate::Process(temp_outline, temp_fill);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void PlasmaFlinger::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(50))
		{
			BaseEntity* target = NULL;
			Core* core_target = root_->GetTarget();
			if(core_target)
			{
				target = core_target;
			}
			else if(_enemies.size() > 0)
			{
				int index = Random::RandomIndex(static_cast<int>(_enemies.size()));
				target = _enemies[index];
			}
			PlasmaSeeker* hm = new PlasmaSeeker(Vector3f(0, 5, 0), target, fill_.GetFillColor(), 1.0f, 1.0f);

			fire_projectile(hm, _spawn_prj);
			cooldown_ = cooldown_time_;
			PowerTick(-20);
			SoundManager::Instance().PlaySample("Fire20.wav");
		}
	}
}

void PlasmaFlinger::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 1315);
	SectionMetadata::RegisterSectionTag(section_type_, "Homing");
}
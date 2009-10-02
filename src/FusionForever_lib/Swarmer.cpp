#include "StdAfx.h"
#include "Swarmer.h"
#include "Core.h"
#include "SwarmMissile.h"
#include "SVGParser.h"
#include "Triangulate.h"

//Initialise all static class members
bool Swarmer::initialised_ = false;
int Swarmer::outline_dl_ = 0;
int Swarmer::outline_verts_index_ = 0;
int Swarmer::fill_dl_ = 0;
int Swarmer::fill_verts_index_ = 0;

Swarmer::Swarmer(void)
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
	cooldown_time_ = 0.15f;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 250;
	section_type_ = "Swarmer";
}

Swarmer::~Swarmer(void)
{
}

void Swarmer::InitialiseGraphics(void)
{
	//Initialise outline and fill from embedded SVG from Inkscape
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	*temp_outline = SVGParser::ParsePath("M 0,-2.5 L 2.5,0 L 2,3 L -2,3 L -2.5,0 L 0,-2.5 z");
	temp_outline->pop_back();

	outline_verts_index_ = Datastore::Instance().AddVerts(temp_outline);
	outline_dl_ = Outlined::CreateOutlinedDisplayList(temp_outline);

	boost::shared_ptr<std::vector<Vector3f>> temp_fill = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());
	Triangulate::Process(temp_outline, temp_fill);

	fill_verts_index_ = Datastore::Instance().AddVerts(temp_fill);
	fill_dl_ = Filled::CreateFillDisplayList(temp_fill);
}

void Swarmer::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	cooldown_ -= _timespan;
	if(firing_)
	{
		if(cooldown_ <= 0.0f && PowerRequirement(5))
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
			SwarmMissile* hm = new SwarmMissile(Vector3f(0, 5, 0), target);

			fire_projectile(hm, _spawn_prj);
			cooldown_ = cooldown_time_;
			PowerTick(-1);
			SoundManager::Instance().PlaySample("Fire3.wav");
		}
	}
}

void Swarmer::RegisterMetadata()
{
	FiringSection::RegisterMetadata();
	SectionMetadata::RegisterSectionKeyValue(section_type_, "Range", 1000);
	SectionMetadata::RegisterSectionTag(section_type_, "Homing");
}
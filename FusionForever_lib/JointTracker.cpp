#include "StdAfx.h"
#include "JointTracker.h"
#include "TurningRoutines.h"
#include "Core.h"
#include "Property.h"

bool JointTracker::initialised_ = false;
int JointTracker::outline_dl_ = 0;
int JointTracker::outline_verts_index_ = 0;
int JointTracker::fill_dl_ = 0;
int JointTracker::fill_verts_index_ = 0;

static const float TURN_RATE = 90.0f;

JointTracker::JointTracker()
: Section()
{
	if(!initialised_)
	{
		InitialiseGraphics();
		initialised_ = true;
	}
	outline_.GetOutlineVerts() = Datastore::Instance().GetVerts(outline_verts_index_);
	outline_.SetDisplayList(outline_dl_);
	fill_.GetFillVerts() = Datastore::Instance().GetVerts(fill_verts_index_);
	fill_.SetDisplayList(fill_dl_);
	target_ = NULL;
	findRadius();

	health_ = FlexFloat(800, 800);
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 200;
	only_when_firing_ = false;
	section_type_ = "JointTracker";
}

JointTracker::~JointTracker()
{
	if(target_ != NULL)
		target_->RemoveSubscriber(this);
}

void JointTracker::InitialiseGraphics(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-2.5f, 0, 0));	//0
	temp_outline->push_back(Vector3f(0, 2.5f, 0));	//1
	temp_outline->push_back(Vector3f(2.5f, 0, 0));	//2
	temp_outline->push_back(Vector3f(0, -2.5f, 0)); //3

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

void JointTracker::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	Core_ptr core_target = root_->GetTarget();
	if(core_target && target_ != core_target)
	{
		if(target_)
			target_->RemoveSubscriber(this);
		target_ = core_target;
		target_->AddSubscriber(this);
	} 
	if(!target_ && _enemies.size() > 0)
	{
		target_ = _enemies[Random::RandomIndex(static_cast<int>(_enemies.size()))];
		target_->AddSubscriber(this);
	}
	if(target_ && (!only_when_firing_ || firing_))
	{
		TurnData turn_data = GetTurnDirection(GetGlobalAngle(), target_->GetGlobalPosition() - GetGlobalPosition());
		
		angle_ += ClampTurnDirection(turn_data.turn_factor, 0.01f) * TURN_RATE * _timespan;
	}
}

void JointTracker::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
		target_ = NULL;
}

void JointTracker::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("OnlyWhenFiring", only_when_firing_ ? "1" : "0");
}

static void sSetOnlyWhenFiring(Section_ptr _section, int _value){static_cast<JointTracker*>(_section)->SetOnlyWhenFiring(_value);}
static int sGetOnlyWhenFiring(Section_ptr _section){return static_cast<JointTracker*>(_section)->GetOnlyWhenFiring();}

void JointTracker::GetProperties(std::vector<Property*>& _properties)
{
	Section::GetProperties(_properties);
	Enumeration e;
	e[0] = "False";
	e[1] = "True";
	_properties.push_back(new Property(this, sSetOnlyWhenFiring, sGetOnlyWhenFiring, e, "Only while firing"));
}

bool JointTracker::ParseSpecific(TiXmlElement* _node)
{
	bool only_when_firing = false;
	//Query parameters specific to JointTrackers
	_node->QueryValueAttribute("OnlyWhenFiring", &only_when_firing);
	only_when_firing_ = only_when_firing;
	return true;
}

void JointTracker::RegisterMetadata()
{
	Section::RegisterMetadata();
	SectionMetadata::RegisterSectionTag(section_type_, "Joint");
	SectionMetadata::RegisterSectionTag(section_type_, "Tracker");
}
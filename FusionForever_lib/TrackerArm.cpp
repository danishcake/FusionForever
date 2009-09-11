#include "StdAfx.h"
#include "TrackerArm.h"
#include "TurningRoutines.h"
#include "Core.h"
#include "Property.h"

bool TrackerArm::initialised_ = false;
int TrackerArm::outline_dl_ = 0;
int TrackerArm::outline_verts_index_ = 0;
int TrackerArm::fill_dl_ = 0;
int TrackerArm::fill_verts_index_ = 0;

static const float TURN_RATE = 90.0f;

TrackerArm::TrackerArm()
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
	angle_range_ = 30.0f;
	angle_centre_ = angle_;
	turn_rate_ = TURN_RATE;
	section_type_ = "TrackerArm";
}

TrackerArm::~TrackerArm()
{
	if(target_ != NULL)
		target_->RemoveSubscriber(this);
}

void TrackerArm::InitialiseGraphics(void)
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

void TrackerArm::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
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
		
		angle_ += ClampTurnDirection(turn_data.turn_factor, 0.1f) * turn_rate_ * _timespan;
		if(angle_ < angle_centre_ - angle_range_)
			angle_ = angle_centre_ - angle_range_;
		if(angle_ > angle_centre_ + angle_range_)
			angle_ = angle_centre_ + angle_range_;
		
	}
}

void TrackerArm::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
		target_ = NULL;
}

void TrackerArm::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("TurnRate", boost::lexical_cast<std::string, float>(turn_rate_));
	_node->SetAttribute("AngleRange", boost::lexical_cast<std::string, float>(angle_range_));
	_node->SetAttribute("OnlyWhenFiring", boost::lexical_cast<std::string, bool>(only_when_firing_));
	
}

static void sSetOnlyWhenFiring(Section_ptr _section, int _value){static_cast<TrackerArm*>(_section)->SetOnlyWhenFiring(_value);}
static int sGetOnlyWhenFiring(Section_ptr _section){return static_cast<TrackerArm*>(_section)->GetOnlyWhenFiring();}

static void sSetAngleRange(Section_ptr _section, float _value){static_cast<TrackerArm*>(_section)->SetAngleRange(_value);}
static float sGetAngleRange(Section_ptr _section){return static_cast<TrackerArm*>(_section)->GetAngleRange();}

static void sSetTurnRate(Section_ptr _section, float _value){static_cast<TrackerArm*>(_section)->SetTurnRate(_value);}
static float sGetTurnRate(Section_ptr _section){return static_cast<TrackerArm*>(_section)->GetTurnRate();}

void TrackerArm::GetProperties(std::vector<Property*>& _properties)
{
	Section::GetProperties(_properties);
	Enumeration e;
	e[0] = "False";
	e[1] = "True";
	_properties.push_back(new Property(this, sSetOnlyWhenFiring, sGetOnlyWhenFiring, e, "Only while firing"));

	_properties.push_back(new Property(this, sSetAngleRange, sGetAngleRange, "Angle range"));
	_properties.push_back(new Property(this, sSetTurnRate, sGetTurnRate, "Turn rate"));
}

bool TrackerArm::ParseSpecific(TiXmlElement* _node)
{
	bool only_when_firing = false;
	float angle_range;
	float turn_rate;
	//Query parameters specific to JointTrackers
	if(_node->QueryValueAttribute("OnlyWhenFiring", &only_when_firing) == TIXML_SUCCESS)
		only_when_firing_ = only_when_firing;
	if(_node->QueryValueAttribute("AngleRange", &angle_range) == TIXML_SUCCESS)
		angle_range_ = angle_range;
	if(_node->QueryValueAttribute("TurnRate", &turn_rate) == TIXML_SUCCESS)
		turn_rate_ = turn_rate;
	return true;
}

void TrackerArm::RegisterMetadata()
{
	Section::RegisterMetadata();
	SectionMetadata::RegisterSectionTag(section_type_, "Joint");
}
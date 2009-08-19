#include "StdAfx.h"
#include "SpinningJoint.h"
#include "Property.h"

bool SpinningJoint::initialised_ = false;
int SpinningJoint::outline_dl_ = 0;
int SpinningJoint::outline_verts_index_ = 0;
int SpinningJoint::fill_dl_ = 0;
int SpinningJoint::fill_verts_index_ = 0;

SpinningJoint::SpinningJoint()
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
	findRadius();

	health_ = FlexFloat(800, 800);
	degrees_per_second_ = 90;
	default_sub_section_position_ = Vector3f(0, 0, 0);
	mass_ = 200;
	section_type_ = "SpinningJoint";
}

SpinningJoint::~SpinningJoint()
{
	
}

void SpinningJoint::InitialiseGraphics(void)
{
	boost::shared_ptr<std::vector<Vector3f>> temp_outline = boost::shared_ptr<std::vector<Vector3f>>(new std::vector<Vector3f>());

	temp_outline->push_back(Vector3f(-2.5f, 0, 0));	//0
	temp_outline->push_back(Vector3f(0, 2.5f, 0));	//1
	temp_outline->push_back(Vector3f(2.5f, 0, 0));		//2
	temp_outline->push_back(Vector3f(0, -2.5f, 0));//3

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

void SpinningJoint::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec, Matrix4f _transform, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	angle_ += _timespan * degrees_per_second_;
}

static float sGetSpinRate(Section_ptr _section){return static_cast<SpinningJoint*>(_section)->GetSpinRate();}
static void sSetSpinRate(Section_ptr _section, float _value){static_cast<SpinningJoint*>(_section)->SetSpinRate(_value);}

void SpinningJoint::GetProperties(std::vector<Property*>& _properties )
{
	Section::GetProperties(_properties);
	_properties.push_back(new Property(this, sSetSpinRate, sGetSpinRate, "Spin rate"));
}

void SpinningJoint::ToXML(TiXmlElement* _node)
{
	Section::ToXML(_node);
	_node->SetAttribute("RotationRate", boost::lexical_cast<std::string, float>(degrees_per_second_));
}

bool SpinningJoint::ParseSpecific(TiXmlElement* _node)
{
	float degrees_per_second = 45;
	_node->QueryFloatAttribute("RotationRate", &degrees_per_second);
	degrees_per_second_ = degrees_per_second;
	return true;
}
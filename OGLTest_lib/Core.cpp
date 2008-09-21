#include "StdAfx.h"
#include <TinyXML.h>
#include "Core.h"

#include "SquareCore.h"
#include "TinyCore.h"
#include "RotatingAI.h"

#include "JointAngles.h"
#include "JointTracker.h"
#include "SpinningJoint.h"
#include "Blaster.h"
#include "HeatBeamGun.h"
#include "HomingMissileLauncher.h"
#include "Swarmer.h"

#include "XMLSection.h"

static const float CORE_ROT_RATE_MAX = 400.0f;
static const float CORE_MOVE_RATE_MAX = 300.0f;
static const float CORE_MOVE_EXP_BRAKING = 10.0f;
static const float CORE_ROT_RATE_DELTA = 500000000.0f;
static const float CORE_ACCL_RATE =      1000000.0f;
static const float CORE_EXP_BRAKING =   3000.0f;

static const float CORE_BASIC_MASS = 1000.0f;
static const float CORE_TOP_MASS = 20000.0;

#define STR_ME( X ) ( # X )
/*
 * Maps the Core type strings used in XML files onto enums
 */
namespace Core_types
{
	static enum Enum
	{
		UnknownCore,
		SquareCore,
		TinyCore
	};

	static const std::string ToStr[] = {
		STR_ME( UnknownCore ),
		STR_ME( SquareCore ),
		STR_ME( TinyCore ),
		};

	static std::map<std::string, Enum> core_type_map;

	static Enum FromStr(std::string _type)
	{
		if(core_type_map.find(_type) != core_type_map.end())
			return core_type_map[_type];
		else
			return UnknownCore;
	}

	static void InitialiseMap()
	{
		core_type_map[ToStr[SquareCore]] = SquareCore;
		core_type_map[ToStr[TinyCore]] = TinyCore;
	}

	static bool initialised = false;
}

/*
 * Maps the section types strings used in XML files onto enums
 */
namespace Section_types
{
	static enum Enum
	{
		UnknownSection,
		JointAngles,
		JointTracker,
		SpinningJoint,
		Blaster,
		HeatBeamGun,
		HomingMissileLauncher,
		Swarmer
	};

	static const std::string ToStr[] = {
		STR_ME( UnknownSection ),
		STR_ME( JointAngles ),
		STR_ME( JointTracker ),
		STR_ME( SpinningJoint ),
		STR_ME( Blaster ),
		STR_ME( HeatBeamGun ),
		STR_ME( HomingMissileLauncher ),
		STR_ME( Swarmer ),
		};

	static std::map<std::string, Enum> section_type_map;

	static Enum FromStr(std::string _type)
	{
		if(section_type_map.find(_type) != section_type_map.end())
			return section_type_map[_type];
		else
			return UnknownSection;
	}

	static void InitialiseMap()
	{
		section_type_map[ToStr[JointAngles]]			= JointAngles;
		section_type_map[ToStr[JointTracker]]			= JointTracker;
		section_type_map[ToStr[SpinningJoint]]			= SpinningJoint;
		section_type_map[ToStr[Blaster]]				= Blaster;
		section_type_map[ToStr[HeatBeamGun]]			= HeatBeamGun;
		section_type_map[ToStr[HomingMissileLauncher]]	= HomingMissileLauncher;
		section_type_map[ToStr[Swarmer]]				= Swarmer;
	}

	static bool initialised = false;
}


Core::Core(BaseAI* _AI)
:Section()
{
	AI_ = _AI;
	energy_ = FlexFloat(100, 100);
	thrust_ = FlexFloat(100);
	target_ = NULL;
}

Core::~Core(void)
{
	delete AI_;
	if(target_ != NULL)
		target_->RemoveSubscriber(this);
}

void Core::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec,
					  Matrix4f _transform, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	//Do all the standard moving and rotating
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	energy_+= 50 * _timespan;
	//Get the AI instructions (how to move, rotate and fire)
	AIAction action;
	float max_speed;
	float max_spin;
	if(mass_ <= CORE_BASIC_MASS)
	{
		max_speed = CORE_MOVE_RATE_MAX * thrust_.GetValue() / 100.0f ;
		max_spin = CORE_ROT_RATE_MAX * thrust_.GetValue() / 100.0f;
	}
	else if(mass_ <= CORE_TOP_MASS)
	{
		max_speed = CORE_MOVE_RATE_MAX * (1.0f - (((mass_ - CORE_BASIC_MASS )/ (CORE_TOP_MASS-CORE_BASIC_MASS)) * 0.95f))* thrust_.GetValue() / 100.0f;
		max_spin = CORE_ROT_RATE_MAX   * (1.0f - (((mass_ - CORE_BASIC_MASS )/ (CORE_TOP_MASS-CORE_BASIC_MASS)) * 0.95f))* thrust_.GetValue() / 100.0f;
	}
	else
	{
		max_speed = CORE_MOVE_RATE_MAX * 0.05f * thrust_.GetValue() / 100.0f;
		max_spin = CORE_ROT_RATE_MAX   * 0.05f * thrust_.GetValue() / 100.0f;
	}
	if(AI_ != NULL)
	{
		action = AI_->Tick(_timespan, _allies, _enemies, this);
		
		if(action.target_ != NULL && target_ != action.target_)
		{
			if(target_ != NULL)
				target_->RemoveSubscriber(this);
			target_ = action.target_;
			target_->AddSubscriber(this);
		}
		Vector2f dv= Vector2f(action.dx_, action.dy_);
		if(dv.lengthSq()!=0)
			dv.normalize();
		velocity_.x += dv.x * _timespan * CORE_ACCL_RATE * (thrust_.GetValue() / 100.0f) / mass_;
		velocity_.y += dv.y * _timespan * CORE_ACCL_RATE * (thrust_.GetValue() / 100.0f) / mass_;
		if(dv.lengthSq() <= 0.01f)
		{
			velocity_ *= expf(-_timespan * CORE_EXP_BRAKING * (thrust_.GetValue() / 100.0f) / mass_);
		}
		if(velocity_.length() > max_speed)
		{
			float excess_speed = velocity_.length() - max_speed;
			float decay_factor = expf(-_timespan * CORE_MOVE_EXP_BRAKING);
			velocity_.normalize();
			velocity_ *= (max_speed + (excess_speed * decay_factor));
		}

		spin_ += action.dtheta_ * _timespan * CORE_ROT_RATE_DELTA / moment_;

		if(spin_ > max_spin)
			spin_ = max_spin;
		if(spin_ < - max_spin)
			spin_ = -max_spin;
		firing_ = action.firing_;
	}
}

void Core::OverrideAI(BaseAI* _new_AI)
{
	delete AI_;
	AI_ = _new_AI;
}

Core_ptr Core::CreateCore(std::string _name)
{
	if(!Core_types::initialised)
	{
		Core_types::InitialiseMap();
		Section_types::InitialiseMap();
	}

	std::string file_name;
	if(_name.find("Scripts/Ships") == std::string::npos)
		file_name = "Scripts/Ships/" + _name + ".xmlShip";
	else
		file_name = _name + ".xmlShip";

	TiXmlDocument ship_document = TiXmlDocument(file_name.c_str());
	if(ship_document.LoadFile())
	{
		TiXmlHandle section_handle = TiXmlHandle(&ship_document);
		TiXmlElement* core_element = section_handle.FirstChild("Section").Element();
		if(core_element)
		{
			Core_ptr core = NULL;
			ParseShip(core_element, ((Section_ptr*)&core));
			if(core)
				return core;
		}
		else
		{
			Logger::LogError(std::string("Ship :") + file_name + std::string(" has no root section"));
			return NULL;
		}
	} else
	{
		Logger::LogError(std::string("Unable to open file '") + file_name + std::string("' :") + std::string(ship_document.ErrorDesc()));
	}

	return NULL;
}
bool Core::ParseShip(TiXmlElement* _section, Section_ptr* _parent)
{
	Section_ptr self = NULL;
	//If *_parent==NULL then it's a root and we have to set root
	if(!(*_parent))
	{
		Core_ptr n_core = ParseCore(_section);
		if(n_core)
		{
			*_parent = n_core;
			self  = *_parent;
		}
		else
			return false; //Error!
	} else
	{
		Section_ptr n_section = ParseSection(_section);
		if(n_section)
		{
			(*_parent)->AddChild(n_section);
			self = n_section;
		}
		else
			return false; //Error!
	}
	//Consider children
	for(TiXmlElement* child_section = _section->FirstChildElement("Section"); child_section != NULL; child_section = child_section->NextSiblingElement())
	{
		if(!ParseShip(child_section, &self))
			return false;
	}


	return true;
}

Core_ptr Core::ParseCore(TiXmlElement* _core_element)
{
	Core_ptr core = NULL;
	std::string core_string;
	if(_core_element->QueryValueAttribute("SectionType", &core_string) == TIXML_SUCCESS)
	{
		//Lookup Core in map of hardcoded Cores
		Core_types::Enum core_type = Core_types::FromStr(core_string);
		switch(core_type)
		{
			case Core_types::SquareCore:
				core = new SquareCore(new RotatingAI(0.5f));
				break;
			case Core_types::TinyCore:
				core = new TinyCore(new RotatingAI(0.5f));
				break;
			default:
				Logger::LogError("Unable to find core type" + core_string);
				break;
		}
		if(core)
		{
			//Now query any standard section atttributes
			ParseCommon(_core_element, core);
			//Now query any core only section atttributes
			
		}
	}

	return core;
}

Section_ptr Core::ParseSection(TiXmlElement* _section_element)
{
	Section_ptr section = NULL;
	std::string section_string;
	if(_section_element->QueryValueAttribute("SectionType", &section_string) == TIXML_SUCCESS)
	{
		//Lookup section in map of hardcoded Cores
		Section_types::Enum section_type = Section_types::FromStr(section_string);
		switch(section_type)
		{
			case Section_types::JointAngles:
				{
					float first_angle = 30;
					float second_angle = -30;
					float transition_time = 1;
					float pause_time = 1;
					//Query parameters specific to JointAngles
					_section_element->QueryFloatAttribute("FirstAngle", &first_angle);
					_section_element->QueryFloatAttribute("SecondAngle", &second_angle);
					_section_element->QueryFloatAttribute("TransitionTime", &transition_time);
					_section_element->QueryFloatAttribute("PauseTime", &pause_time);
					section = new JointAngles(first_angle, second_angle, transition_time, pause_time);
				}
				break;
			case Section_types::JointTracker:
				{
					bool only_when_firing = false;
					//Query parameters specific to JointTrackers
					_section_element->QueryValueAttribute("OnlyWhenFiring", &only_when_firing);
					section = new JointTracker(only_when_firing);
				}
				break;
			case Section_types::SpinningJoint:
				{
					float degrees_per_second = 45;
					_section_element->QueryFloatAttribute("RotationRate", &degrees_per_second);
					section = new SpinningJoint(degrees_per_second);
				}
				break;
			case Section_types::Blaster:
				section = new Blaster();
				break;
			case Section_types::HeatBeamGun:
				section = new HeatBeamGun();
				break;
			case Section_types::HomingMissileLauncher:
				section = new HomingMissileLauncher();
				break;
			case Section_types::Swarmer:
				section = new Swarmer();
				break;
			default:
				//Attempt to find XMLSection
				section = XMLSection::CreateXMLSection(section_string);
				if(!section)
				{
					//Can't find, log error
					Logger::LogError("Unable to find section type" + section_string);
				}
				break;
		}
		if(section)
		{
			//Now query any standard section atttributes
			ParseCommon(_section_element, section);
			//Now query any non core attributes
			Vector3f position = section->GetPosition();
			if((_section_element->QueryValueAttribute("x", &position.x) == TIXML_SUCCESS) |
			   (_section_element->QueryValueAttribute("y", &position.y) == TIXML_SUCCESS))
			   section->SetPosition(position);
		}
	}

	return section;
}

void Core::ParseCommon(TiXmlElement* _section_element, Section* _section)
{
	//Now query any standard section atttributes
	float health = 100;
	if(_section_element->QueryValueAttribute("Health", &health) == TIXML_SUCCESS)
		_section->SetMaxHealth(health);
	float angle = 0;
	if(_section_element->QueryValueAttribute("Angle", &angle) == TIXML_SUCCESS)
		_section->SetAngle(angle);
	float delay = 0;
	if(_section_element->QueryValueAttribute("Delay", &delay) == TIXML_SUCCESS)
		_section->SetFiringDelay(delay);
}

void Core::SaveCore(std::string _filename)
{

}

void Core::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
		target_ = NULL;
}
#include "StdAfx.h"
#include "Core.h"

#include "XMLCore.h"

#include "RotatingAI.h"

#include "JointAngles.h"
#include "JointTracker.h"
#include "SpinningJoint.h"
#include "TrackerArm.h"
#include "Blaster.h"
#include "HeatBeamGun.h"
#include "HomingMissileLauncher.h"
#include "Swarmer.h"
#include "ChainGun.h"
#include "PlasmaArtillery.h"

#include "SectionTypes.h"
#include "XMLSection.h"

static const float CORE_ROT_RATE_MAX = 400.0f;
static const float CORE_MOVE_RATE_MAX = 300.0f;
static const float CORE_MOVE_EXP_BRAKING = 10.0f;
static const float CORE_ROT_RATE_DELTA = 500000000.0f;
static const float CORE_ACCL_RATE =      1000000.0f;
static const float CORE_EXP_BRAKING =   3000.0f;

static const float CORE_BASIC_MASS = 1000.0f;
static const float CORE_TOP_MASS = 20000.0;


Core::Core(BaseAI* _AI)
:Section()
{
	AI_ = _AI;
	energy_ = FlexFloat(100, 100);
	thrust_ = FlexFloat(100);
	target_ = NULL;
	total_damage_ = 0;
	total_shield_damage_ = 0;
	total_health_ = health_.GetMaxValue();
	loss_of_thrust_factor_ = 0;
	last_damage_time_;
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

	//Get the AI instructions (how to move, rotate and fire)
	AIAction action;
	float max_speed;
	float max_spin;

	if(mass_ <= CORE_BASIC_MASS)
	{
		max_speed = CORE_MOVE_RATE_MAX * (thrust_.GetValue() / 100.0f);
		max_spin = CORE_ROT_RATE_MAX * (thrust_.GetValue() / 100.0f);
	}
	else if(mass_ <= CORE_TOP_MASS)
	{
		max_speed = CORE_MOVE_RATE_MAX * (1.0f - (((mass_ - CORE_BASIC_MASS )/ (CORE_TOP_MASS-CORE_BASIC_MASS)) * 0.95f))* (thrust_.GetValue() / 100.0f);
		max_spin = CORE_ROT_RATE_MAX   * (1.0f - (((mass_ - CORE_BASIC_MASS )/ (CORE_TOP_MASS-CORE_BASIC_MASS)) * 0.95f))* (thrust_.GetValue() / 100.0f);
	}
	else
	{
		max_speed = CORE_MOVE_RATE_MAX * 0.05f * thrust_.GetValue() / 100.0f;
		max_spin = CORE_ROT_RATE_MAX   * 0.05f * thrust_.GetValue() / 100.0f;
	}
	if(AI_ != NULL)
	{
		action = AI_->Tick(_timespan, _allies, _enemies, this);
		
		float effective_thrust = thrust_.GetValue() / 100.0f;
		if(action.thrust_ && PowerRequirement(25))
		{
			effective_thrust *= 3.0f;
			energy_ -= 10.0f * effective_thrust * _timespan;
			max_speed *= 1.5f;
			max_spin *= 1.5f;
		}
		float damage_fraction = GetTotalDamage() / GetTotalHealth();
		if(damage_fraction > 0.7f)
		{
			float slow_factor = 1.0f - 0.95f *((damage_fraction - 0.7f) / 0.3f);
			slow_factor = slow_factor > 1.0f ? 1.0f : slow_factor < 0.05f ? 0.05f : slow_factor;
			max_speed = max_speed * slow_factor;
			max_spin = max_spin * slow_factor;
			effective_thrust *= slow_factor;
		}

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
		velocity_.x += dv.x * _timespan * CORE_ACCL_RATE * effective_thrust / mass_;
		velocity_.y += dv.y * _timespan * CORE_ACCL_RATE * effective_thrust / mass_;
		if(dv.lengthSq() <= 0.01f)
		{
			velocity_ *= expf(-_timespan * CORE_EXP_BRAKING * effective_thrust / mass_);
		}
		if(velocity_.length() > max_speed)
		{
			float excess_speed = velocity_.length() - max_speed;
			float decay_factor = expf(-_timespan * CORE_MOVE_EXP_BRAKING);
			velocity_.normalize();
			velocity_ *= (max_speed + (excess_speed * decay_factor));
		}

		spin_ += action.dtheta_ * _timespan * CORE_ROT_RATE_DELTA / moment_;

		if(spin_ > 2 * max_spin)
			spin_ = 2 * max_spin;
		if(spin_ < -2 * max_spin)
			spin_ = -2 * max_spin;
		if(spin_ > max_spin)
			spin_ = max_spin + (spin_ - max_spin) * expf(-_timespan * CORE_MOVE_EXP_BRAKING);
		if(spin_ < -max_spin)
			spin_ = -max_spin + (spin_ + max_spin) * expf(-_timespan * CORE_MOVE_EXP_BRAKING);
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
	std::string file_name = _name;

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
			Logger::ErrorOut() << "Ship :" << file_name << " has no root section\n";
			return NULL;
		}
	} else
	{
		Logger::ErrorOut() << "Unable to open file '" << file_name << "' :" <<
			ship_document.ErrorDesc() << "(" << boost::lexical_cast<std::string, int>(ship_document.ErrorRow()) <<
			":" << boost::lexical_cast<std::string, int>(ship_document.ErrorCol()) << ")\n";
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
		core = XMLCore::CreateXMLCore(core_string);
		if(!core)
			Logger::ErrorOut() << "Unable to open core:" << core_string << "\n";
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
		section = SectionTypes::GetSection(section_string);
		if(!section)
		{
			//Attempt to find XMLSection
			section = XMLSection::CreateXMLSection(section_string);
			if(!section)
			{
				//Can't find, log error
				Logger::ErrorOut() << "Unable to find section type" << section_string << "\n";
			}
		}

		if(section)
		{
			//Now query any standard section atttributes
			ParseCommon(_section_element, section);
			section->ParseSpecific(_section_element);
			//Now query any non core attributes
			Vector3f position = section->GetPosition();
			if((_section_element->QueryValueAttribute("x", &position.x) == TIXML_SUCCESS) |
			   (_section_element->QueryValueAttribute("y", &position.y) == TIXML_SUCCESS))
			   section->SetPosition(position);
		}
	} else
	{
		Logger::ErrorOut() << "Unable to create section, SectionType attribute is blank\n";
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

void Core::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
		target_ = NULL;
}

void Core::ReportDamage(float _damage, float /*_shield_damage*/)
{
	total_damage_ += _damage;
	last_damage_time_ = 0;
}

void Core::RegisterMetadata()
{
	Section::RegisterMetadata();
	SectionMetadata::RegisterSectionTag(section_type_, "Core");
}

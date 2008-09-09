#include "StdAfx.h"
#include <TinyXML.h>
#include "Core.h"

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
}

Core::~Core(void)
{
	delete AI_;
}

void Core::Tick(float _timespan, std::vector<Projectile_ptr>& _spawn_prj, std::vector<Decoration_ptr>& _spawn_dec,
					  Matrix4f _transform, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, ICollisionManager* _collision_manager)
{
	//Do all the standard moving and rotating
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform, _enemies, _collision_manager);
	this->PowerTick(50 * _timespan);
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
	std::string file_name = std::string(_name);
	file_name = "Scripts/Ships/" + file_name + ".xmlShip";

	TiXmlDocument ship_document = TiXmlDocument(file_name.c_str());
	if(ship_document.LoadFile())
	{
		TiXmlHandle section_handle = TiXmlHandle(&ship_document);
		//ParseShip(section_handle.FirstChildElement(
		
	} else
	{
		Logger::LogError(std::string("Unable to open file '") + file_name + std::string("' :") + std::string(ship_document.ErrorDesc()));
	}
	return NULL;
}
bool Core::ParseShip(TiXmlElement* _section)
{
	return true;
}
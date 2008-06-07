#include "StdAfx.h"
#include "Core.h"

static const float CORE_ROT_RATE_MAX = 600.0f;
static const float CORE_MOVE_RATE_MAX = 200.0f;
static const float CORE_ACCL_RATE = 300.0f;
static const float CORE_EXP_BRAKING = 2.0f;


Core::Core(BaseAI* _AI)
:Section()
{
	AI_ = _AI;
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
	//Get the AI instructions (how to move, rotate and fire)
	AIAction action;
	if(AI_!=NULL)
	{
		action = AI_->Tick(_timespan, _allies, _enemies, this);
		Vector2f dv= Vector2f(action.dx_, action.dy_);
		if(dv.lengthSq()!=0)
			dv.normalize();
		velocity_.x += dv.x * _timespan * CORE_ACCL_RATE;
		velocity_.y += dv.y * _timespan * CORE_ACCL_RATE;
		if(dv.lengthSq() <= 0.01f)
		{
			velocity_ *= expf(-_timespan * CORE_EXP_BRAKING);
		}
		if(velocity_.length()>CORE_MOVE_RATE_MAX)
		{
			velocity_.normalize();
			velocity_*=CORE_MOVE_RATE_MAX;
		}

		angle_ += action.dtheta_ * _timespan * CORE_ROT_RATE_MAX;
		firing_ = action.firing_;
	}
}

void Core::OverrideAI(BaseAI* _new_AI)
{
	delete AI_;
	AI_ = _new_AI;
}
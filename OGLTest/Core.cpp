#include "StdAfx.h"
#include "Core.h"

#define CORE_ROT_RATE_MAX 600.0f
#define CORE_MOVE_RATE_MAX 200.0f
#define CORE_ACCL_RATE 300.0f
#define CORE_EXP_BRAKING 2.0f


Core::Core(BaseAI* _AI)
:Section()
{
	AI_ = _AI;
}

Core::~Core(void)
{
	delete AI_;
}

void Core::Tick(float _timespan, std::list<boost::shared_ptr<Projectile>>& _spawn_prj, std::list<boost::shared_ptr<Decoration>>& _spawn_dec, 
				Matrix4f _transform, std::list<boost::shared_ptr<Core>>& _allies, std::list<boost::shared_ptr<Core>>& _enemies)
{
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform);
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
#include "StdAfx.h"
#include "Core.h"

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
	AIAction action;
	if(AI_!=NULL)
	{
		action = AI_->Tick(_timespan, _allies, _enemies);
		Vector2f dv= Vector2f(action.dx_, action.dy_);
		if(dv.lengthSq()!=0)
			dv.normalize();
		velocity_.x += dv.x * _timespan * CORE_MOVE_RATE_MAX;
		velocity_.y += dv.y * _timespan * CORE_MOVE_RATE_MAX;
		angle_ += action.dtheta_ * _timespan * CORE_ROT_RATE_MAX;
		firing_ = action.firing_;
		
	}
	Section::Tick(_timespan, _spawn_prj, _spawn_dec, _transform);
}
#include "StdAfx.h"
#include "SimpleAI.h"
#include "TurningRoutines.h"

static const float SimpleAIFocusTime = 10;
static const float SimpleAIStrafeTime = 3;
static const float SimpleAIMinimumRange = 100;

SimpleAI::SimpleAI()
{
	sum_time_ = 0;
	focus_time_ = 0;
	max_focus_time_ = Random::RandomCentered(SimpleAIFocusTime, 3);
	strafe_time_ = 0;
	strafe_clockwise_ = Random::RandomChance(0.5f);
	target_ = NULL;
	preferred_minimum_range_= SimpleAIMinimumRange;
}

SimpleAI::~SimpleAI(void)
{
	if(target_ != NULL)
	{
		target_->RemoveSubscriber(this);
	}
}

AIAction SimpleAI::Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self)
{
	AIAction a = AIAction(0, 0, 0, false); 

	sum_time_ += _timespan;
	focus_time_ += _timespan;
	strafe_time_ += _timespan;
	if(strafe_time_ > SimpleAIStrafeTime)
	{
		strafe_time_ = 0;
		if(Random::RandomChance(0.4f))
			strafe_clockwise_ = !strafe_clockwise_;
		if(Random::RandomChance(0.8f))
			preferred_minimum_range_ = SimpleAIMinimumRange * 1.5;
		else
			preferred_minimum_range_ = SimpleAIMinimumRange * 1;
	}

	if(focus_time_ > max_focus_time_ || target_ == NULL)
	{
		focus_time_ = 0;
		//Pick a new target
		if(_enemies.size() > 0)
		{
			if(target_ != NULL)
				target_->RemoveSubscriber(this);
			int index = Random::RandomIndex(_enemies.size());

			target_ = _enemies[index];
			target_->AddSubscriber(this);
		}
	}	

	if(target_ != NULL)
	{
		Vector3f relative_position = (target_->GetPosition() - _self->GetPosition());
		float range = relative_position.length();
		float angle = atan2f(relative_position.x, relative_position.y);
		if(relative_position.lengthSq() > 0)
			relative_position.normalize();
		if(range > preferred_minimum_range_ * 2) //Close on target
		{
			a.dx_ = sinf(angle);
			a.dy_ = cosf(angle);
		}else if(range < preferred_minimum_range_) //Back off target
		{
			a.dx_ = -sinf(angle);
			a.dy_ = -cosf(angle);
		} else //Circle strafe
		{
			//Add sideways movement
			if(strafe_clockwise_)
			{
				a.dx_ = -cosf(angle) * 0.2f;
				a.dy_ = sinf(angle) * 0.2f;
			} else
			{
				a.dx_ = cosf(angle) * 0.2f;
				a.dy_ = -sinf(angle) * 0.2f;
			}
			//Reduce range movement
			float movement_in_range_dimension;
			Vector3f self_velocity = _self->GetVelocity();
			if(self_velocity.lengthSq() > 0)
			{
				self_velocity.normalize();
				movement_in_range_dimension = self_velocity.dotProduct(relative_position);
				a.dx_ -= 0.8f * sinf(angle) * movement_in_range_dimension;
				a.dy_ -= 0.8f * cosf(angle) * movement_in_range_dimension;
			}
		}
		TurnData td = GetTurnDirection(_self->GetAngle(), relative_position);
		td.turn_factor = ClampTurnDirection(td.turn_factor, 0.1f);
		//td.turn_factor *= 0.2f;
		a.dtheta_ = td.turn_factor;
		a.firing_ = true;
	}

	return a;
}

void SimpleAI::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
	{
		target_ = NULL;
	}
}
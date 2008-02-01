#include "StdAfx.h"
#include "RotatingAI.h"

RotatingAI::RotatingAI(float _rotation_rate)
{
	sum_time_ = 0;
	rotation_rate_ = _rotation_rate;
	if(rotation_rate_ < - 1)
		rotation_rate_ = -1;
	if(rotation_rate_ > 1)
		rotation_rate_ = 1;
	fire_for_next_ = 0;
}

RotatingAI::~RotatingAI(void)
{
}

AIAction RotatingAI::Tick(float _timespan, std::list<Core_ptr>& _allies, std::list<Core_ptr>& _enemies, Core* _self)
{
	sum_time_ += _timespan;
	fire_for_next_ -= _timespan;
	if((float)rand()/(float)RAND_MAX < 0.1 * _timespan)
		fire_for_next_ = 3.0f;
	return AIAction(0, 0, rotation_rate_, fire_for_next_ > 0);
}
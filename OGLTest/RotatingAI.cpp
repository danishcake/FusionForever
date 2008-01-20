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
	
}

RotatingAI::~RotatingAI(void)
{
}

AIAction RotatingAI::Tick(float _timespan, std::list<Core_ptr>& _allies, std::list<Core_ptr>& _enemies, Core* _self)
{
	sum_time_ += _timespan;
	return AIAction(0, 0, rotation_rate_, false);
}
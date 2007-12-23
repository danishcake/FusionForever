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

AIAction RotatingAI::Tick(float _timespan, std::list<boost::shared_ptr<Core>>& _allies, std::list<boost::shared_ptr<Core>>& _enemies)
{
	sum_time_ += _timespan;
	return AIAction(0, 0, rotation_rate_, false);
}
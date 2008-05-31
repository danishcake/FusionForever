#include "StdAfx.h"
#include "SimpleAI.h"

static const float SimpleAIFocusTime = 10;

SimpleAI::SimpleAI(float _rotation_rate)
{
	sum_time_ = 0;	 
}

SimpleAI::~SimpleAI(void)
{
}

AIAction SimpleAI::Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self)
{
	sum_time_ += _timespan;
	focus_time_ += _timespan;
	if(focus_time_ > SimpleAIFocusTime)
	{
		focus_time_ = 0;
		//Pick a new target
		
	}
	AIAction a = AIAction(0, 0, 0, 360, false); 

	return a;
}

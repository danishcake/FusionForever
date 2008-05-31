#pragma once
#include "Core.h"

class SimpleAI :
	public BaseAI
{
protected:
	float sum_time_;
	float focus_time_;
public:
	SimpleAI(float _rotation_rate);
	virtual ~SimpleAI(void);
	virtual AIAction Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self);
};

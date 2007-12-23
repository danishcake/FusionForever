#pragma once
#include "Core.h"

class RotatingAI :
	public BaseAI
{
protected:
	float sum_time_;
	float rotation_rate_;
public:
	RotatingAI(float _rotation_rate);
	virtual ~RotatingAI(void);
	virtual AIAction Tick(float _timespan, std::list<boost::shared_ptr<Core>>& _allies, std::list<boost::shared_ptr<Core>>& _enemies, Core* _self);
};

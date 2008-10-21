#pragma once
#include "BaseAI.h"

class RotatingAI :
	public BaseAI
{
protected:
	float sum_time_;
	float rotation_rate_;
	float fire_for_next_; //n seconds of firing, triggered randomly
public:
	RotatingAI(float _rotation_rate);
	virtual ~RotatingAI(void);
	virtual AIAction Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self);
};

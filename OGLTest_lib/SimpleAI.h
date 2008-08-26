#pragma once
#include "Core.h"

class SimpleAI :
	public BaseAI
{
protected:
	float sum_time_;
	float focus_time_;
	float max_focus_time_;
	float strafe_time_;
	bool strafe_clockwise_;
	float preferred_minimum_range_;
	Core_ptr target_;
public:
	SimpleAI();
	virtual ~SimpleAI(void);
	virtual AIAction Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self);
	virtual void EndSubscription(Subscriber* _source);
};

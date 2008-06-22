#pragma once
#include "Core.h"
#include "sdl.h"

class KeyboardAI :
	public BaseAI
{
protected:
	float zoom_time_;
public:
	KeyboardAI(void);
	virtual ~KeyboardAI(void);
	virtual AIAction Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self);
};

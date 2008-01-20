#pragma once
#include "Core.h"
#include "sdl.h"
#include "vmath.h"

class KeyboardAI :
	public BaseAI
{
protected:
	float zoom_time_;
public:
	KeyboardAI(void);
	virtual ~KeyboardAI(void);
	virtual AIAction Tick(float _timespan, std::list<Core_ptr>& _allies, std::list<Core_ptr>& _enemies, Core* _self);
};

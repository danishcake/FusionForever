#pragma once
#include "Core.h"
#include "sdl.h"
#include "vmath.h"

class KeyboardAI :
	public BaseAI
{
protected:
	float dx_;
	float dy_;
public:
	KeyboardAI(void);
	virtual ~KeyboardAI(void);
	virtual AIAction Tick(float _timespan, std::list<boost::shared_ptr<Core>>& _allies, std::list<boost::shared_ptr<Core>>& _enemies, Core* _self);
};

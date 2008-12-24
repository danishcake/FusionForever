#pragma once
#include "BaseAI.h"
#include "sdl.h"

class KeyboardAI :
	public BaseAI
{
protected:
	float zoom_time_;
public:
	KeyboardAI(void);
	virtual ~KeyboardAI(void);
	virtual AIAction Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self);
	virtual void SpecifyTarget(Core* _target){}
	virtual bool IsHuman(){return true;}
};

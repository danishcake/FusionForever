#pragma once
#include "BaseScene.h"

class IntroScene :
	public BaseScene
{
public:
	IntroScene(void);
	virtual ~IntroScene(void);

	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();

private:
	float timeleft_;
	
	static const float intro_max_time_;
};

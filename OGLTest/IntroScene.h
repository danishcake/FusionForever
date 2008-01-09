#pragma once
#include "basescene.h"

class IntroScene :
	public BaseScene
{
public:
	IntroScene(void);
	virtual ~IntroScene(void);

	virtual void Tick(float _timespan, std::vector<boost::shared_ptr<BaseScene>>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();

private:
	float timeleft_;
	
	static const float intro_max_time_;
};

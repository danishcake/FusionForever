#pragma once
#include "BaseScene.h"

class FadeInScene :
	public BaseScene
{
public:
	FadeInScene();
	virtual ~FadeInScene(void);

	virtual void Tick(float _timespan, std::vector<boost::shared_ptr<BaseScene>>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();

protected:
	float timeleft_;
};

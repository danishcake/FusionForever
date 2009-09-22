#pragma once
#include "BaseScene.h"

/**
  * The FadeInScene represents a fade in and should be push onto the scene vector after
  * the item to be faded in.
  */
class FadeInScene :
	public BaseScene
{
public:
	FadeInScene();
	virtual ~FadeInScene(void);

	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();

	static const float FITime;
protected:
	float timeleft_;
};

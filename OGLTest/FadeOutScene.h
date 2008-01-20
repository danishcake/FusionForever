#pragma once
#include "BaseScene.h"

class FadeOutScene :
	public BaseScene
{
public:
	FadeOutScene(std::vector<BaseScene_ptr> _fadeout_done_scenes);
	FadeOutScene();
	virtual ~FadeOutScene(void);

	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();

	static const float FOTime;
protected:
	float timeleft_;
	std::vector<BaseScene_ptr> fadeout_done_scenes_;
	bool return_end_scene;
};

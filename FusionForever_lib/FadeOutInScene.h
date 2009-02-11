#pragma once
#include "BaseScene.h"

class FadeOutInScene :
	public BaseScene
{
public:
	FadeOutInScene(std::vector<BaseScene_ptr> _fadeout_done_scenes);
	virtual ~FadeOutInScene(void);

	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();
	
	static const float FOTime;
protected:
	float timeleft_;
	std::vector<BaseScene_ptr> fadeout_done_scenes_;
	bool faded_out_;
};

#pragma once
#include "BaseScene.h"

class FadeOutInScene :
	public BaseScene
{
public:
	FadeOutInScene(std::vector<boost::shared_ptr<BaseScene>> _fadeout_done_scenes);
	virtual ~FadeOutInScene(void);

	virtual void Tick(float _timespan, std::vector<boost::shared_ptr<BaseScene>>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();
	
	static const float FOTime;
	static const float FITime;
protected:
	float timeleft_;
	std::vector<boost::shared_ptr<BaseScene>> fadeout_done_scenes_;
	bool faded_out_;
};

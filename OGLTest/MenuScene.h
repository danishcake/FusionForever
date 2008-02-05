#pragma once
#include "BaseScene.h"
#include <AntTweakBar.h>

class MenuScene :
	public BaseScene
{
public:
	MenuScene(void);
	virtual ~MenuScene(void);
	
	virtual void Draw(void);	
	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual bool IsRemovable();
	virtual bool IsRoot();
	void StartChallenge();
protected:
	bool start_challenge_;
	bool fading_out_;

	float sum_time_;
	float fade_out_time_;
	TwBar* gbar;

};

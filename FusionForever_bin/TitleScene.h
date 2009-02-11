#pragma once
#include "BaseScene.h"
#include "BaseGame.h"
#include "Billboard.h"

class TitleScene :
	public BaseScene
{
public:
	TitleScene();

	virtual ~TitleScene();
	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();
protected:
	BaseGame* game_;
	boost::shared_ptr<Billboard> title_billboard_;
	boost::shared_ptr<Billboard> instruction_billboard_;
	bool exit_to_menu_;
	bool finished_script_;
	bool spawned_instruction_;
	bool spawned_fadeout_;
	float logo_timer_;
	float exit_timer_;
};

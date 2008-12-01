#pragma once
#include "BaseScene.h"
#include <CEGUI.h>

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

	bool LoadChallenges(const CEGUI::EventArgs& e);
	bool StartChallenge(const CEGUI::EventArgs& e);
	bool StartEditor(const CEGUI::EventArgs& e);
	bool ExitGame(const CEGUI::EventArgs& e);
	bool cbSettingsOK(const CEGUI::EventArgs& e);
	bool cbSettingsCancel(const CEGUI::EventArgs& e);
	bool cbSettings(const CEGUI::EventArgs& e);
	
protected:
	bool start_challenge_;
	std::string challenge_name_;
	bool start_editor_;
	bool exit_game_;
	bool lock_gui_;
	bool fading_out_;

	float sum_time_;
	float fade_out_time_;
};

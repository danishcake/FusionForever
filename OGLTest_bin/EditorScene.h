#pragma once
#include "BaseScene.h"
#include <CEGUI.h>

class EditorGame;
class Section;

class EditorScene :
	public BaseScene
{
public:
	EditorScene(void);
	virtual ~EditorScene(void);
	
	virtual void Draw(void);	
	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual bool IsRemovable();
	virtual bool IsRoot();

	bool cbReturnToMenu(const CEGUI::EventArgs& e);
	bool cbSave(const CEGUI::EventArgs& e);
	bool cbSetCoreToSquareCore(const CEGUI::EventArgs& e);
	bool cbSetCoreToTinyCore(const CEGUI::EventArgs& e);
	bool cbAddBlaster(const CEGUI::EventArgs& e);
	bool cbAddHeatBeam(const CEGUI::EventArgs& e);
	bool cbAddHomingMissileLauncher(const CEGUI::EventArgs& e);
	bool cbAddSwarmer(const CEGUI::EventArgs& e);
	bool cbAddLuaSection(const CEGUI::EventArgs& e);
	
	bool cbChangeOrientation(const CEGUI::EventArgs& e);
	bool cbChangeAngle(const CEGUI::EventArgs& e);
	bool cbChangeDistance(const CEGUI::EventArgs& e);
	
	bool cbBackgroundClick(const CEGUI::EventArgs& e);

protected:
	bool return_to_menu_;
	bool fading_out_;
	bool lock_gui_;
	float sum_time_;
	float fade_out_time_;
	EditorGame* game_;

	Section* selection_;
};

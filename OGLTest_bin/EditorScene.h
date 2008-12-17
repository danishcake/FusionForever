#pragma once
#include "BaseScene.h"
#include <CEGUI.h>
#include "vmath.h"

class EditorGame;
class Section;

namespace EditorDragMode
{
	enum Enum
	{
		NotDragging, RotateDrag, MoveDrag, ScrollDrag
	};
}

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

	void SetSelected(Section* _selection);

	bool cbReturnToMenu(const CEGUI::EventArgs& e);
	bool cbSave(const CEGUI::EventArgs& e);
	bool cbLoad(const CEGUI::EventArgs& e);
	bool cbDelete(const CEGUI::EventArgs& e);
	bool cbDeleteTree(const CEGUI::EventArgs& e);
	bool cbTry(const CEGUI::EventArgs& e);

	bool cbSaveDialogueSave(const CEGUI::EventArgs& e);
	bool cbSaveDialogueCancel(const CEGUI::EventArgs& e);
	bool cbSaveDialogueListSelected(const CEGUI::EventArgs& e);

	bool cbLoadDialogueLoad(const CEGUI::EventArgs& e);
	bool cbLoadDialogueCancel(const CEGUI::EventArgs& e);

	bool cbSetCoreToXMLCore(const CEGUI::EventArgs& e);
	bool cbAddSection(const CEGUI::EventArgs& e);

	bool cbBackgroundClick(const CEGUI::EventArgs& e);
	bool cbBackgroundMove(const CEGUI::EventArgs& e);
	bool cbBackgroundMBD(const CEGUI::EventArgs& e);
	bool cbBackgroundMBU(const CEGUI::EventArgs& e);
	bool cbBackgroundMouseLeave(const CEGUI::EventArgs& e);

	bool cbPropertyChanged(const CEGUI::EventArgs& e);
	bool cbEnumeratedPropertyChanged(const CEGUI::EventArgs& e);

protected:
	bool return_to_menu_;
	bool try_challenge_;
	bool fading_out_;
	bool lock_gui_;
	float sum_time_;
	float fade_out_time_;
	EditorGame* game_;

	CEGUI::Point old_position;
	Vector3f accumulated_snap;
	bool move_first_tick;

	Section* selection_;
	EditorDragMode::Enum drag_mode_;

	std::string filename_;
};

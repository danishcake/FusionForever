#pragma once
#include "vmath.h"
#include "BaseGame.h"
class Section;
class Core;

class EditorGame :
	public BaseGame
{
private:
	Section* selected_section_;

public:
	EditorGame(void);
	virtual ~EditorGame(void);

	Section* GetAtMouseCoord(Vector3f _point);
	std::vector<Section*> EditorGame::GetAllAtMouseCoord(Vector3f _point);
	void SetCore(Core* _core);
	void LoadCore(Core* _core);
	Core* GetCore();
	
	virtual void Draw();
	virtual int Tick(float _timespan, GameGUI& _gui);

	void SetSelectedSection(Section* _selected_section)
	{
		selected_section_ = _selected_section;
	}
};

#pragma once
#include "vmath.h"
#include "BaseGame.h"
class Section;
class Core;

class EditorGame :
	public BaseGame
{
public:
	EditorGame(void);
	virtual ~EditorGame(void);

	Section* GetAtMouseCoord(Vector3f _point);
	void SetCore(Core* _core);
	Core* GetCore();
};

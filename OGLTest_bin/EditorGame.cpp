#include "StdAfx.h"
#include "EditorGame.h"

#include "SquareCore.h" //Pulls in Core.h
#include "RotatingAI.h"

EditorGame::EditorGame(void)
{
	AddShip(new SquareCore(new RotatingAI(0.05f)), 0);
}

EditorGame::~EditorGame(void)
{
}

void EditorGame::SetCore(Core* _core)
{
	std::vector<Section_ptr> children = ships_[0].at(0)->DetachChildren();
	delete ships_[0].at(0);
	ships_[0].clear();

	ships_[0].push_back(_core);
	ships_[0].at(0)->AttachChildren(children);
}

Core* EditorGame::GetCore()
{
	return ships_[0].at(0);
}

Section_ptr EditorGame::GetAtMouseCoord(Vector3f _point)
{
	Section_ptr clicked_section = NULL;
	ships_[0].at(0)->CheckCollisions(_point, clicked_section);
	return clicked_section;
}
#include "StdAfx.h"
#include "EditorGame.h"

#include "XMLCore.h"
#include "RotatingAI.h"

EditorGame::EditorGame(void) : BaseGame("")
{
	AddShip(XMLCore::CreateXMLCore("SquareCore"), 1);
	selected_section_ = ships_[0].at(0);
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

void EditorGame::LoadCore(Core* _core)
{
	delete ships_[0].at(0);
	ships_[0].clear();
	ships_[0].push_back(_core);
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

std::vector<Section_ptr> EditorGame::GetAllAtMouseCoord(Vector3f _point)
{
	std::vector<Section_ptr> clicked_sections;
	ships_[0].at(0)->CheckCollisions(_point, clicked_sections);
	return clicked_sections;
}

void EditorGame::Draw()
{
	BaseGame::Draw();
	Vector3f scaled_line = Camera::Instance().ScreenDeltaToWorldDelta(Vector3f(5,0,0));
	ships_[0].at(0)->DrawEditorSupport(scaled_line.x, selected_section_);
}
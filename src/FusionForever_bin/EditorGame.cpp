#include "StdAfx.h"
#include "EditorGame.h"

#include "XMLCore.h"
#include "RotatingAI.h"
#include <sdl.h>
#include "LuaAI.h"
#include "LuaChallenge.h"

EditorGame::EditorGame(void) : BaseGame("")
{
	Core_ptr core = XMLCore::CreateXMLCore("SquareCore");
	if(core)
	{
		core->OverrideAI(LuaAI::FromScript("EditorAI.luaAI", challenge_->GetLuaAICache()));
		AddShip(core, 0);
		selected_section_ = ships_[0].at(0);
	} else
	{
		Logger::ErrorOut() << "Unable to create a SquareCore for the editor, so exiting to avoid a crash. Ensure SquareCore.xmlCore is present in Scripts/Sections\n";
		SDL_Quit();
		exit(-1);
	}
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
	_core->OverrideAI(LuaAI::FromScript("EditorAI.luaAI", challenge_->GetLuaAICache()));
}

void EditorGame::LoadCore(Core* _core)
{
	delete ships_[0].at(0);
	ships_[0].clear();
	ships_[0].push_back(_core);
	_core->OverrideAI(LuaAI::FromScript("EditorAI.luaAI", challenge_->GetLuaAICache()));
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

int EditorGame::Tick(float _timespan, GameGUI& _gui)
{
	//Stop ship rotating 
	if(ships_[0][0])
	{
		ships_[0][0]->SetPosition(Vector3f(0, 0, 0));
		ships_[0][0]->SetAngle(0);
	}

	return BaseGame::Tick(_timespan, _gui);
}
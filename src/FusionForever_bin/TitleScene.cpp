#include "StdAfx.h"
#include "TitleScene.h"
#include "MenuScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"

#include <boost/foreach.hpp>
#include "ICollisionManager.h"
#include "GridCollisionManager.h"
#include "TextureLoader.h"
#include "LuaChallenge.h"
#include "GameGui.h"
#include <functional>
#include <sdl.h>

TitleScene::TitleScene()
{
	Camera::Instance().SetWidth(500);
	game_ = new BaseGame("Scripts/Challenges/Intro.luaChallenge");
	for(int i = 0; i < 4; i++)
		Camera::Instance().ZoomOut();

	logo_timer_ = 0;
	exit_timer_ = 0;
	exit_to_menu_ = false;
	finished_script_ = false;
	spawned_fadeout_ = false;
	spawned_instruction_ = false;
}

TitleScene::~TitleScene(void)
{
	delete game_;
}

void TitleScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	GameGUI gui;

	/* Check for any keypresses and if found skip intro */
	Uint8* keystates = SDL_GetKeyState(0);
	if(!exit_to_menu_)
	{
		for(int key = 0; key < SDLK_LAST; key++)
		{
			if(key != SDLK_NUMLOCK &&
			   key != SDLK_CAPSLOCK && 
			   key != SDLK_SCROLLOCK && 
			   key!= SDLK_F9 && //For fraps/taksi
			   keystates[key] )
			{
				exit_to_menu_ = true;
			}
		}
	}

	ChallengeState::Enum state = (ChallengeState::Enum)game_->Tick(_timespan, gui);

	if(state == ChallengeState::Draw && !finished_script_)
	{
		finished_script_ = true;
		title_billboard_ = boost::shared_ptr<Billboard>(new Billboard("FusionForever", BillboardType::ScreenSpace));
		title_billboard_->SetSize(Vector2f(512, 256));
		title_billboard_->SetColor(GLColor(255, 255, 255, 0.0f));
		title_billboard_->SetPosition(Vector3f(static_cast<float>(Camera::Instance().GetWindowWidth()) / 2.0f, static_cast<float>(Camera::Instance().GetWindowHeight()) / 2.0f, 0));
	}

	if(finished_script_)
		logo_timer_ += _timespan;
	if(logo_timer_ > 1.5f && !spawned_instruction_)
	{
		instruction_billboard_ = boost::shared_ptr<Billboard>(new Billboard("PressAnyKey", BillboardType::ScreenSpace));
		instruction_billboard_->SetSize(Vector2f(256, 128));
		instruction_billboard_->SetColor(GLColor(255, 255, 255, 0.0f));
		instruction_billboard_->SetOffset(Vector2f(1.1f, 1));
		instruction_billboard_->SetPosition(Vector3f(static_cast<float>(Camera::Instance().GetWindowWidth()), static_cast<float>(Camera::Instance().GetWindowHeight()), 0));
	}
	if(exit_to_menu_)
		exit_timer_ += _timespan;
	if(exit_to_menu_ && !spawned_fadeout_)
	{
		std::vector<BaseScene_ptr> fo_done_scenes;
		fo_done_scenes.push_back(BaseScene_ptr(new MenuScene()));
		fo_done_scenes.push_back(BaseScene_ptr(new FadeInScene()));
		BaseScene_ptr fo = BaseScene_ptr(new FadeOutScene(fo_done_scenes));
		_new_scenes.push_back(fo);
		spawned_fadeout_ = true;		
	}
}

void TitleScene::Draw()
{
	game_->Draw();

	//Cause the billboard to fade in
	if(title_billboard_.get() != NULL)
	{
		float alpha = logo_timer_ / 0.5f;
		if(alpha > 1.0f)
			alpha = 1.0f;
		title_billboard_->SetColor(GLColor(255, 255, 255, alpha));
		title_billboard_->Draw();
	}
	if(instruction_billboard_.get() != NULL)
	{
		float alpha_factor = (logo_timer_ - 1.5f) / 0.3f;
		if(alpha_factor > 1.0f)
			alpha_factor = 1.0f;
		float alpha = (0.2f * sinf((logo_timer_ - 1.5f) * 5.0f) + 0.8f) * alpha_factor;
		instruction_billboard_->SetColor(GLColor(255,255,255, alpha));
		instruction_billboard_->Draw();
	}
}

bool TitleScene::IsRoot()
{
	return true;
}

bool TitleScene::IsRemovable()
{
	return exit_timer_ >= FadeOutScene::FOTime * 0.9f;
}

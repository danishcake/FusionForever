#include "StdAfx.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"
#include <sdl.h>

void MenuScene::StartChallenge()
{
	if(fading_out_)
		return;
	
	start_challenge_ = true;
	fade_out_time_ = 1.0f;
	fading_out_ = true;
}

void TW_CALL btnStartChallenge(void *clientData)
{
	((MenuScene*)clientData)->StartChallenge();
}

void TW_CALL btnExit(void *clientData)
{
	SDL_Event* exit_event = new SDL_Event();
	exit_event->type = SDL_QUIT;
	SDL_PushEvent(exit_event);
}

void TW_CALL btnNotImplemented(void *clientData)
{
}


MenuScene::MenuScene(void)
{
	gbar = TwNewBar("Main Menu");
	TwAddButton(gbar,"Start Challenge", btnStartChallenge, this, NULL);
	TwAddButton(gbar,"Ship creator",btnNotImplemented, this, NULL);
	TwAddSeparator(gbar,"-",NULL);
	TwAddButton(gbar, "Exit", btnExit, this, NULL);
	start_challenge_ = false;
	fading_out_ = false;
	fade_out_time_ = 0;
	sum_time_ = 0;
}

void MenuScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	sum_time_ += _timespan;
	fade_out_time_ -= _timespan;
	if(start_challenge_)	//Fade out of MenuScene and in with GameScene
	{
		start_challenge_ = false;
		std::vector<BaseScene_ptr> fo_done_scenes;
		fo_done_scenes.push_back(BaseScene_ptr(new GameScene()));
		fo_done_scenes.push_back(BaseScene_ptr(new FadeInScene()));
		BaseScene_ptr fo = BaseScene_ptr(new FadeOutScene(fo_done_scenes));
		_new_scenes.push_back(fo);
	}
}

MenuScene::~MenuScene(void)
{
	TwDeleteBar(gbar);
}

void MenuScene::Draw()
{
	
}

bool MenuScene::IsRemovable()
{
	return (fading_out_ && (fade_out_time_ < 0));
}

bool MenuScene::IsRoot()
{
	return true;
}
#include "StdAfx.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"

void MenuScene::StartChallenge()
{
	if(fading_out_ > 0)
		return;
	
	start_challenge_ = true;
	fade_out_time_ = 1.0f;
	fading_out_ = true;
}

void TW_CALL btnStartChallenge(void *clientData)
{
	((MenuScene*)clientData)->StartChallenge();
}

MenuScene::MenuScene(void)
{
	gbar = TwNewBar("Main Menu");
	TwAddButton(gbar,"Start Challenge", btnStartChallenge, this, NULL);
	start_challenge_ = false;
	fading_out_ = false;
	fade_out_time_ = 0;
	sum_time_ = 0;
}

void MenuScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	sum_time_ += _timespan;
	fade_out_time_ -= _timespan;
	if(start_challenge_)
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
	TwDraw();
}

bool MenuScene::IsRemovable()
{
	return (fading_out_ && (fade_out_time_ < 0));
}

bool MenuScene::IsRoot()
{
	return true;
}
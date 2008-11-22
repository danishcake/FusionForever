#include "StdAfx.h"
#include "GameScene.h"
#include "MenuScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"

#include <boost/foreach.hpp>
#include "ICollisionManager.h"
#include "GridCollisionManager.h"

GameScene::GameScene(std::string _challenge)
{
	Camera::Instance().SetWidth(500);
	game_ = new BaseGame(_challenge + ".luaChallenge");
	for(int i = 0; i < 4; i++)
		Camera::Instance().ZoomOut();

	timer_ = 0;
	returning_to_menu_ = false;
	spawn_fadeout_ = false;
}

GameScene::~GameScene(void)
{
	delete game_;
}

void GameScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	if(game_->Tick(_timespan))
	{
		returning_to_menu_ = true;
	}

	if(returning_to_menu_)
		timer_ += _timespan;
	if(timer_ > 3.0f && !spawn_fadeout_)
	{
		spawn_fadeout_ = true;
		std::vector<BaseScene_ptr> fo_done_scenes;
		fo_done_scenes.push_back(BaseScene_ptr(new MenuScene()));
		fo_done_scenes.push_back(BaseScene_ptr(new FadeInScene()));
		BaseScene_ptr fo = BaseScene_ptr(new FadeOutScene(fo_done_scenes));
		_new_scenes.push_back(fo);
	}
}

void GameScene::Draw()
{
	game_->Draw();
}

bool GameScene::IsRoot()
{
	return true;
}

bool GameScene::IsRemovable()
{
	return timer_ >= 4.0f;
}

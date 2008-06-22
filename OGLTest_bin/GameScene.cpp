#include "StdAfx.h"
#include "GameScene.h"

#include <boost/foreach.hpp>
#include "ICollisionManager.h"
#include "GridCollisionManager.h"

GameScene::GameScene(void)
{
	Camera::Instance().SetWidth(500);
	game_ = new BaseGame();
	game_->LoadChallenge("Challenge1.lua");
}

GameScene::~GameScene(void)
{
	delete game_;
}

void GameScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	game_->Tick(_timespan);
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
	return false;
}

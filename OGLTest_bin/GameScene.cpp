#include "StdAfx.h"
#include "GameScene.h"
#include "MenuScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"

#include <boost/foreach.hpp>
#include "ICollisionManager.h"
#include "GridCollisionManager.h"
#include "TextureLoader.h"
#include "LuaChallenge.h"
#include "Scorekeeper.h"

GameScene::GameScene(std::string _challenge)
{
	Camera::Instance().SetWidth(500);
	game_ = new BaseGame(_challenge + ".luaChallenge");
	for(int i = 0; i < 4; i++)
		Camera::Instance().ZoomOut();

	timer_ = 0;
	returning_to_menu_ = false;
	spawn_fadeout_ = false;
	challenge_ = _challenge;
}

GameScene::~GameScene(void)
{
	delete game_;
}

void GameScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	ChallengeState::Enum state = (ChallengeState::Enum)game_->Tick(_timespan);
	if(!returning_to_menu_)
	{
		if(state == ChallengeState::Victory)
		{
			returning_to_menu_ = true;
			end_billboard_ = boost::shared_ptr<Billboard>(new Billboard("Victory.texture", BillboardType::ScreenSpace));
			end_billboard_->SetPosition(Vector3f(Camera::Instance().GetWindowWidth()/2, Camera::Instance().GetWindowHeight()/2, 0));
			Scorekeeper::Instance().ReportVictory(challenge_);
		} else if(state == ChallengeState::Defeat)
		{
			returning_to_menu_ = true;
			end_billboard_ = boost::shared_ptr<Billboard>(new Billboard("Defeat.texture", BillboardType::ScreenSpace));
			end_billboard_->SetPosition(Vector3f(Camera::Instance().GetWindowWidth()/2, Camera::Instance().GetWindowHeight()/2, 0));
			Scorekeeper::Instance().ReportDefeat(challenge_);
		} else if(state ==ChallengeState::Draw)
		{
			returning_to_menu_ = true;
		}
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


	if(timer_ > 1.0f)
	{
		//Cause the billboard to fade in
		if(timer_ < 1.3)
			glColor4f(1.0f, 1.0f, 1.0f, (timer_ - 1.0f) / 0.3f);
		else
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		end_billboard_->Draw();
	}


}

bool GameScene::IsRoot()
{
	return true;
}

bool GameScene::IsRemovable()
{
	return timer_ >= 4.0f;
}


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
	sum_time_ = 0;
	player_id_ = -1;

	Camera::Instance().SetWidth(500);
	game_ = new BaseGame(_challenge + ".luaChallenge");
	for(int i = 0; i < 4; i++)
		Camera::Instance().ZoomOut();

	timer_ = 0;
	returning_to_menu_ = false;
	returning_to_editor_ = false;
	spawn_fadeout_ = false;
	challenge_ = _challenge;

	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* myRoot = wmgr.createWindow( "DefaultWindow", "Game/Root" );
	
	
	CEGUI::DefaultWindow* energy_text = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/StaticText", "Game/Root/EnergyText" );
	energy_text->setPosition(CEGUI::UVector2(CEGUI::UDim(0.075f,0 ),CEGUI::UDim(0, 5)));
	energy_text->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0, 25)));
	energy_text->setText("Energy");
	energy_text->setProperty( "BackgroundEnabled","False");
	myRoot->addChildWindow(energy_text);

	CEGUI::ProgressBar* energy_bar = (CEGUI::ProgressBar*)wmgr.createWindow( "TaharezLook/ProgressBar", "Game/Root/EnergyBar" );
	energy_bar->setPosition(CEGUI::UVector2(CEGUI::UDim(0.075f,0 ),CEGUI::UDim(0, 30)));
	energy_bar->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0, 20)));
	energy_bar->setProgress(0.5f);
	myRoot->addChildWindow(energy_bar);

	CEGUI::DefaultWindow* core_text = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/StaticText", "Game/Root/CoreText" );
	core_text->setPosition(CEGUI::UVector2(CEGUI::UDim(0.375f,0 ),CEGUI::UDim(0, 5)));
	core_text->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0, 25)));
	core_text->setText("Core Health");
	myRoot->addChildWindow(core_text);

	CEGUI::ProgressBar* core_bar = (CEGUI::ProgressBar*)wmgr.createWindow( "TaharezLook/ProgressBar", "Game/Root/CoreBar" );
	core_bar->setPosition(CEGUI::UVector2(CEGUI::UDim(0.375f,0 ),CEGUI::UDim(0, 30)));
	core_bar->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0, 20)));
	core_bar->setProgress(0.5f);
	myRoot->addChildWindow(core_bar);

	CEGUI::DefaultWindow* ship_text = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/StaticText", "Game/Root/ShipText" );
	ship_text->setPosition(CEGUI::UVector2(CEGUI::UDim(0.675f,0 ),CEGUI::UDim(0, 5)));
	ship_text->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0, 25)));
	ship_text->setText("Ship Health");
	myRoot->addChildWindow(ship_text);

	CEGUI::ProgressBar* ship_bar = (CEGUI::ProgressBar*)wmgr.createWindow( "TaharezLook/ProgressBar", "Game/Root/ShipBar" );
	ship_bar->setPosition(CEGUI::UVector2(CEGUI::UDim(0.675f,0 ),CEGUI::UDim(0, 30)));
	ship_bar->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0, 20)));
	ship_bar->setProgress(0.5f);
	myRoot->addChildWindow(ship_bar);

	energy_bar->setVisible(false);
	ship_bar->setVisible(false);
	core_bar->setVisible(false);

}

GameScene::~GameScene(void)
{
	CEGUI::WindowManager::getSingleton().destroyWindow("Game/Root");
	delete game_;
}

void GameScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	if(sum_time_ == 0)
		CEGUI::System::getSingleton().setGUISheet( CEGUI::WindowManager::getSingleton().getWindow("Game/Root"));
	int games_player_id = game_->GetPlayerSectionID();
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::ProgressBar* p_eb = (CEGUI::ProgressBar*)wmgr.getWindow("Game/Root/EnergyBar");
	CEGUI::ProgressBar* p_sb = (CEGUI::ProgressBar*)wmgr.getWindow("Game/Root/ShipBar");
	CEGUI::ProgressBar* p_cb = (CEGUI::ProgressBar*)wmgr.getWindow("Game/Root/CoreBar");
	CEGUI::DefaultWindow* p_ct = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/CoreText" );
	CEGUI::DefaultWindow* p_st = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/ShipText" );
	CEGUI::DefaultWindow* p_et = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/EnergyText" );

	if(player_id_ != games_player_id)
	{
		Core_ptr p_core = static_cast<Core_ptr>(game_->GetSectionData(games_player_id));
		if(games_player_id != -1 && p_core)
		{
			p_eb->setVisible(true);
			p_sb->setVisible(true);
			p_cb->setVisible(true);
			p_ct->setVisible(true);
			p_st->setVisible(true);
			p_et->setVisible(true);

			player_id_ = games_player_id;
		} else
		{
			p_eb->setVisible(false);
			p_sb->setVisible(false);
			p_cb->setVisible(false);
			p_ct->setVisible(false);
			p_st->setVisible(false);
			p_et->setVisible(false);

			player_id_ = -1;
		}
	}

	if(player_id_ != -1)
	{
		if(Core_ptr p_core = static_cast<Core_ptr>(game_->GetSectionData(player_id_)))
		{
			float ship_health = 1.0f - (p_core->GetTotalDamage() / p_core->GetTotalHealth());
			p_sb->setProgress(ship_health);
			p_eb->setProgress(p_core->GetEnergy().GetValue() / p_core->GetEnergy().GetMaxValue());
			p_cb->setProgress(p_core->GetHealth() / p_core->GetMaxHealth());
		} else 
		{
			p_eb->setVisible(false);
			p_sb->setVisible(false);
			p_cb->setVisible(false);
			p_ct->setVisible(false);
			p_st->setVisible(false);
			p_et->setVisible(false);

			player_id_ = -1;
		}
	}


	sum_time_+=_timespan;
	ChallengeState::Enum state = (ChallengeState::Enum)game_->Tick(_timespan);
	if(!returning_to_menu_)
	{
		if(state == ChallengeState::Victory)
		{
			returning_to_menu_ = true;
			end_billboard_ = boost::shared_ptr<Billboard>(new Billboard("Victory", BillboardType::ScreenSpace));
			end_billboard_->SetPosition(Vector3f(static_cast<float>(Camera::Instance().GetWindowWidth()) / 2.0f, static_cast<float>(Camera::Instance().GetWindowHeight()) / 2.0f, 0));
			Scorekeeper::Instance().ReportVictory(challenge_);
		} else if(state == ChallengeState::Defeat)
		{
			returning_to_menu_ = true;
			end_billboard_ = boost::shared_ptr<Billboard>(new Billboard("Defeat", BillboardType::ScreenSpace));
			end_billboard_->SetPosition(Vector3f(static_cast<float>(Camera::Instance().GetWindowWidth()) / 2.0f, static_cast<float>(Camera::Instance().GetWindowHeight()) / 2, 0));
			Scorekeeper::Instance().ReportDefeat(challenge_);
		} else if(state == ChallengeState::Draw)
		{
			returning_to_menu_ = true;
		} else if(state == ChallengeState::ReturnToEditor)
		{
			returning_to_editor_ = true;
		}
	}


	if(returning_to_menu_ || returning_to_editor_)
	{
		timer_ += _timespan;
	}

	if(returning_to_editor_ && !spawn_fadeout_)
	{
		spawn_fadeout_ = true;
		timer_ = 3.0f;
		std::vector<BaseScene_ptr> fo_done_scenes;
		fo_done_scenes.push_back(BaseScene_ptr(new FadeInScene()));
		BaseScene_ptr fo = BaseScene_ptr(new FadeOutScene(fo_done_scenes));
		_new_scenes.push_back(fo);
	}

	if(timer_ > 3.0f && !spawn_fadeout_ && returning_to_menu_)
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
		if(end_billboard_.px != NULL)
		{
			if(timer_ < 1.3f)
				end_billboard_->SetColor(GLColor(255,255,255, (timer_ - 1.0f) / 0.3f)); 
			else
				end_billboard_->SetColor(GLColor(255,255,255, 1.0f));
			if(timer_ < 2.0f)
				end_billboard_->SetSize(Vector2f(512,256) * (timer_-1.0f) / 1.0f);
			else
				end_billboard_->SetSize(Vector2f(512,256));
			end_billboard_->Draw();
		}
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

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
#include "GameGui.h"
#include <functional>




GameScene::GameScene(std::string _challenge)
{
	sum_time_ = 0;

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
	message_log_ = CEGUIMessageLog(myRoot);
	
	CEGUI::DefaultWindow* energy_text = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/StaticText", "Game/Root/EnergyText" );
	energy_text->setPosition(CEGUI::UVector2(CEGUI::UDim(0.075f,0 ),CEGUI::UDim(0.025f, 0)));
	energy_text->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0.05f, 0)));
	energy_text->setText("Energy");
	energy_text->setProperty("BackgroundEnabled", "False");
	myRoot->addChildWindow(energy_text);

	CEGUI::ProgressBar* energy_bar = (CEGUI::ProgressBar*)wmgr.createWindow( "TaharezLook/ProgressBar", "Game/Root/EnergyBar" );
	energy_bar->setPosition(CEGUI::UVector2(CEGUI::UDim(0, energy_text->getFont()->getTextExtent("Energy") + 10),CEGUI::UDim(0.1f, 0)));
	energy_bar->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f, -15 - energy_text->getFont()->getTextExtent("Energy")),CEGUI::UDim(0.8f, 0)));
	energy_bar->setProgress(0.5f);
	energy_text->addChildWindow(energy_bar);


	CEGUI::DefaultWindow* core_text = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/StaticText", "Game/Root/CoreText" );
	core_text->setPosition(CEGUI::UVector2(CEGUI::UDim(0.375f,0 ),CEGUI::UDim(0.025f, 0)));
	core_text->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0.05f, 0)));
	core_text->setText("Core Health");
	core_text->setProperty("BackgroundEnabled", "False");
	myRoot->addChildWindow(core_text);

	CEGUI::ProgressBar* core_bar = (CEGUI::ProgressBar*)wmgr.createWindow( "TaharezLook/ProgressBar", "Game/Root/CoreBar" );
	core_bar->setPosition(CEGUI::UVector2(CEGUI::UDim(0, core_text->getFont()->getTextExtent("Core Health") + 10),CEGUI::UDim(0.1f, 0)));
	core_bar->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f, -15 - core_text->getFont()->getTextExtent("Core Health")),CEGUI::UDim(0.8f, 0)));
	core_bar->setProgress(0.5f);
	core_text->addChildWindow(core_bar);

	CEGUI::DefaultWindow* ship_text = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/StaticText", "Game/Root/ShipText" );
	ship_text->setPosition(CEGUI::UVector2(CEGUI::UDim(0.675f,0 ),CEGUI::UDim(0.025f, 0)));
	ship_text->setSize(CEGUI::UVector2(CEGUI::UDim(0.25f, 0),CEGUI::UDim(0.05f, 0)));
	ship_text->setText("Ship Health");
	ship_text->setProperty("BackgroundEnabled", "False");
	myRoot->addChildWindow(ship_text);

	CEGUI::ProgressBar* ship_bar = (CEGUI::ProgressBar*)wmgr.createWindow( "TaharezLook/ProgressBar", "Game/Root/ShipBar" );
	ship_bar->setPosition(CEGUI::UVector2(CEGUI::UDim(0, ship_text->getFont()->getTextExtent("Ship Health") + 10),CEGUI::UDim(0.1f, 0)));
	ship_bar->setSize(CEGUI::UVector2(CEGUI::UDim(1.0f, -15 - ship_text->getFont()->getTextExtent("Ship Health")),CEGUI::UDim(0.8f, 0)));
	ship_bar->setProgress(0.5f);
	ship_text->addChildWindow(ship_bar);

	CEGUI::DefaultWindow* counter_A = (CEGUI::DefaultWindow*)wmgr.createWindow( "TaharezLook/StaticText", "Game/Root/CounterA" );
	counter_A->setPosition(CEGUI::UVector2(CEGUI::UDim(0.9f, 0), CEGUI::UDim(0.05f, 0)));
	counter_A->setSize(CEGUI::UVector2(CEGUI::UDim(0.09f, 0), CEGUI::UDim(0.05f, 0)));
	counter_A->setProperty("BackgroundEnabled", "False");
	counter_A->setProperty("HorzFormatting", "RightAligned");
	counter_A->setProperty("FrameEnabled", "False");
	counter_A->setVisible(false);
	myRoot->addChildWindow(counter_A);

	CEGUI::DefaultWindow* counter_B = (CEGUI::DefaultWindow*)wmgr.createWindow( "TaharezLook/StaticText", "Game/Root/CounterB" );
	counter_B->setPosition(CEGUI::UVector2(CEGUI::UDim(0.9f, 0), CEGUI::UDim(0.1f, 0)));
	counter_B->setSize(CEGUI::UVector2(CEGUI::UDim(0.09f, 0), CEGUI::UDim(0.05f, 0)));
	counter_B->setProperty("BackgroundEnabled", "False");
	counter_B->setProperty("HorzFormatting", "RightAligned");
	counter_B->setProperty("FrameEnabled", "False");
	counter_B->setVisible(false);
	myRoot->addChildWindow(counter_B);

	CEGUI::DefaultWindow* counter_C = (CEGUI::DefaultWindow*)wmgr.createWindow( "TaharezLook/StaticText", "Game/Root/CounterC" );
	counter_C->setPosition(CEGUI::UVector2(CEGUI::UDim(0.9f, 0), CEGUI::UDim(0.15f, 0)));
	counter_C->setSize(CEGUI::UVector2(CEGUI::UDim(0.09f, 0), CEGUI::UDim(0.05f, 0)));
	counter_C->setProperty("BackgroundEnabled", "False");
	counter_C->setProperty("HorzFormatting", "RightAligned");
	counter_C->setProperty("FrameEnabled", "False");
	counter_C->setVisible(false);
	myRoot->addChildWindow(counter_C);

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
	
	sum_time_+=_timespan;
	GameGUI gui;

	ChallengeState::Enum state = (ChallengeState::Enum)game_->Tick(_timespan, gui);
	//Add gui messages
	BOOST_FOREACH(ScreenText st, gui.new_messages_)
	{
		message_log_.AddMessage(st);
	}
	message_log_.Tick(_timespan);

	//Update bars
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::ProgressBar* p_eb = (CEGUI::ProgressBar*)wmgr.getWindow("Game/Root/EnergyBar");
	CEGUI::ProgressBar* p_sb = (CEGUI::ProgressBar*)wmgr.getWindow("Game/Root/ShipBar");
	CEGUI::ProgressBar* p_cb = (CEGUI::ProgressBar*)wmgr.getWindow("Game/Root/CoreBar");
	CEGUI::DefaultWindow* p_ct = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/CoreText" );
	CEGUI::DefaultWindow* p_st = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/ShipText" );
	CEGUI::DefaultWindow* p_et = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/EnergyText" );
	CEGUI::DefaultWindow* p_cA = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/CounterA" );
	CEGUI::DefaultWindow* p_cB = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/CounterB" );
	CEGUI::DefaultWindow* p_cC = (CEGUI::DefaultWindow*)wmgr.getWindow("Game/Root/CounterC" );

	if(gui.render_bars && !ltv_gui_.render_bars)
	{
		p_eb->setVisible(true);
		p_sb->setVisible(true);
		p_cb->setVisible(true);
		p_ct->setVisible(true);
		p_st->setVisible(true);
		p_et->setVisible(true);
	}
	if(gui.ship_health_fraction != ltv_gui_.ship_health_fraction)
		p_sb->setProgress(gui.ship_health_fraction);
	if(gui.energy_fraction != ltv_gui_.energy_fraction)
		p_eb->setProgress(gui.energy_fraction);
	if(gui.core_health_fraction != ltv_gui_.core_health_fraction)
		p_cb->setProgress(gui.core_health_fraction);

	if(ltv_gui_.render_bars && !gui.render_bars)
	{
		p_eb->setVisible(false);
		p_sb->setVisible(false);
		p_cb->setVisible(false);
		p_ct->setVisible(false);
		p_st->setVisible(false);
		p_et->setVisible(false);
	}
	//Update counters
	if(gui.counter_A_visible && !ltv_gui_.counter_A_visible)
		p_cA->setVisible(true);
	if(!gui.counter_A_visible && ltv_gui_.counter_A_visible)
		p_cA->setVisible(false);
	if(ltv_gui_.counter_A_value != gui.counter_A_value || ltv_gui_.counter_A_max != gui.counter_A_max)
	{
		if(gui.counter_A_max != -1)
			p_cA->setText((boost::lexical_cast<std::string, int>(gui.counter_A_value) + std::string("/") + boost::lexical_cast<std::string, int>(gui.counter_A_max)).c_str());
		else
			p_cA->setText((boost::lexical_cast<std::string, int>(gui.counter_A_value)).c_str());
	}
	if(gui.counter_B_visible && !ltv_gui_.counter_B_visible)
		p_cB->setVisible(true);
	if(!gui.counter_B_visible && ltv_gui_.counter_B_visible)
		p_cB->setVisible(false);
	if(ltv_gui_.counter_B_value != gui.counter_B_value || ltv_gui_.counter_B_max != gui.counter_B_max)
	{
		if(gui.counter_B_max != -1)
			p_cB->setText((boost::lexical_cast<std::string, int>(gui.counter_B_value) + std::string("/") + boost::lexical_cast<std::string, int>(gui.counter_B_max)).c_str());
		else
			p_cB->setText((boost::lexical_cast<std::string, int>(gui.counter_B_value)).c_str());
	}

	if(gui.counter_C_visible && !ltv_gui_.counter_C_visible)
		p_cC->setVisible(true);
	if(!gui.counter_C_visible && ltv_gui_.counter_C_visible)
		p_cC->setVisible(false);
	if(ltv_gui_.counter_C_value != gui.counter_C_value || ltv_gui_.counter_C_max != gui.counter_C_max)
	{
		if(gui.counter_C_max != -1)
			p_cC->setText((boost::lexical_cast<std::string, int>(gui.counter_C_value) + std::string("/") + boost::lexical_cast<std::string, int>(gui.counter_C_max)).c_str());
		else
			p_cC->setText((boost::lexical_cast<std::string, int>(gui.counter_C_value)).c_str());
	}


	ltv_gui_ = gui;

	if(!returning_to_menu_ && !returning_to_editor_)
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
		if(end_billboard_.get() != NULL)
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
	return timer_ >= 3.0f + FadeOutScene::FOTime * 0.9f;
}

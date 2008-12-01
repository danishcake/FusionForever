#include "StdAfx.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "EditorScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"
#include <sdl.h>

#include <boost/filesystem.hpp>


bool MenuScene::StartChallenge(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;
	const CEGUI::WindowEventArgs& we = 	static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::Listbox* pChallengeList = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("Menu/LevelsList");
	if(pChallengeList->getSelectedCount() == 1)
	{
		challenge_name_ = pChallengeList->getFirstSelectedItem()->getText().c_str();
	}

	start_challenge_ = true;
	lock_gui_ = true;
	fade_out_time_ = 1.0f;
	fading_out_ = true;

	return true;
}

bool MenuScene::StartEditor(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;
	
	start_editor_ = true;
	lock_gui_ = true;
	fade_out_time_ = 1.0f;
	fading_out_ = true;

	return true;
}

bool MenuScene::cbSettingsOK(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pWndSettings = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Menu/Settings");
	pWndSettings->setVisible(false);
	pWndSettings->setModalState(false);
	return true;
}

bool MenuScene::cbSettingsCancel(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pWndSettings = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Menu/Settings");
	pWndSettings->setVisible(false);
	pWndSettings->setModalState(false);
	return true;
}

bool MenuScene::cbSettings(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;

	CEGUI::Window* pWndSettings = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Menu/Settings");
	pWndSettings->setVisible(true);
	pWndSettings->setModalState(true);

	return true;
}

bool MenuScene::ExitGame(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;
	
	exit_game_ = true;
	lock_gui_ = true;
	fade_out_time_ = 1.0f;
	fading_out_ = true;

	return true;
}

MenuScene::MenuScene(void)
{
	start_challenge_ = false;
	start_editor_ = false;
	exit_game_ = false;
	fading_out_ = false;
	lock_gui_ = false;
	fade_out_time_ = 0;
	sum_time_ = 0;

	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* myRoot = wmgr.createWindow( "DefaultWindow", "Menu/Root" );

	CEGUI::PushButton* pBtnQuit = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/Quit");
	pBtnQuit->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 100 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnQuit->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 20), CEGUI::UDim( 1, -50 ) ) );
	pBtnQuit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::ExitGame, this));
	pBtnQuit->setText( "Quit" );
	myRoot->addChildWindow(pBtnQuit);

	CEGUI::PushButton* pBtnStart = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/Start");
	pBtnStart->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 100 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnStart->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 20), CEGUI::UDim( 0.0f, 20 ) ) );
	pBtnStart->setText( "Start Game" );
	pBtnStart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::StartChallenge, this));
	myRoot->addChildWindow(pBtnStart);

	CEGUI::PushButton* pBtnEditor = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/Editor");
	pBtnEditor->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 100 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnEditor->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 20), CEGUI::UDim( 0.0f, 70 ) ) );
	pBtnEditor->setText( "Ship Editor" );
	pBtnEditor->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::StartEditor, this));
	myRoot->addChildWindow(pBtnEditor);

	CEGUI::PushButton* pBtnSettings = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/SettingsBtn");
	pBtnSettings->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 100 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnSettings->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 20), CEGUI::UDim( 0.0f, 120 ) ) );
	pBtnSettings->setText( "Settings" );
	pBtnSettings->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::cbSettings, this));
	myRoot->addChildWindow(pBtnSettings);

	CEGUI::Window* pWndChallenges = wmgr.createWindow( "TaharezLook/FrameWindow", "Menu/Levels" );
	pWndChallenges->setPosition(CEGUI::UVector2(CEGUI::UDim(0,130),CEGUI::UDim(0,10)));
	pWndChallenges->setSize(CEGUI::UVector2(CEGUI::UDim(1,-140), CEGUI::UDim(1,-20)));
	pWndChallenges->setProperty("SizingEnabled", "False");
	pWndChallenges->setProperty("CloseButtonEnabled", "False");
	pWndChallenges->setProperty("DragMovingEnabled", "False");
	pWndChallenges->setText("Challenges");
	myRoot->addChildWindow(pWndChallenges);

	CEGUI::Listbox* pWndChallengeList = (CEGUI::Listbox*)wmgr.createWindow( "TaharezLook/Listbox", "Menu/LevelsList" );
	pWndChallengeList->setPosition(CEGUI::UVector2(CEGUI::UDim(0,10),CEGUI::UDim(0,40)));
	pWndChallengeList->setSize(CEGUI::UVector2(CEGUI::UDim(1,-20), CEGUI::UDim(1,-50)));
	pWndChallenges->addChildWindow(pWndChallengeList);

	
	const CEGUI::Image* sel_img = &CEGUI::ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush");

	boost::filesystem::directory_iterator end_itr;	
	for(boost::filesystem::directory_iterator itr = boost::filesystem::directory_iterator("./Scripts/Challenges");
		itr != end_itr;
		++itr)
	{
		if(boost::filesystem::is_regular((itr->status())))
		{
			std::string ext = boost::filesystem::extension(*itr);
			if(ext == ".luaChallenge")
			{
				CEGUI::ListboxItem* lbi = new CEGUI::ListboxTextItem(boost::filesystem::basename(itr->path()));
				lbi->setSelectionBrushImage(sel_img);
				pWndChallengeList->addItem(lbi);
			}
		}
	}

	CEGUI::Window* pWndSettings = wmgr.createWindow( "TaharezLook/FrameWindow", "Menu/Settings" );
	pWndSettings->setVisible(false);
	pWndSettings->setModalState(false);
	pWndSettings->setProperty("SizingEnabled", "False");
	pWndSettings->setProperty("CloseButtonEnabled", "False");
	pWndSettings->setProperty("DragMovingEnabled", "False");
	pWndSettings->setText("Settings");
	pWndSettings->setSize( CEGUI::UVector2( CEGUI::UDim( 1, -60 ), CEGUI::UDim( 1, -60 ) ) );
	pWndSettings->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 30 ), CEGUI::UDim( 0, 30 ) ) );
	{
		CEGUI::Window* pBtnOK = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/Settings/OK");
		pBtnOK->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 60 ), CEGUI::UDim( 0, 30 ) ) );
		pBtnOK->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -70 ), CEGUI::UDim( 1, -40 ) ) );
		pBtnOK->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::cbSettingsOK, this));
		pBtnOK->setText("Save");
		pWndSettings->addChildWindow(pBtnOK);

		CEGUI::Window* pBtnCancel = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/Settings/Cancel");
		pBtnCancel->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 60 ), CEGUI::UDim( 0, 30 ) ) );
		pBtnCancel->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -140 ), CEGUI::UDim( 1, -40 ) ) );
		pBtnCancel->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::cbSettingsCancel, this));
		pBtnCancel->setText("Cancel");
		pWndSettings->addChildWindow(pBtnCancel);
	}

	myRoot->addChildWindow(pWndSettings);
}

void MenuScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	if(sum_time_ == 0)
		CEGUI::System::getSingleton().setGUISheet( CEGUI::WindowManager::getSingleton().getWindow("Menu/Root"));
	sum_time_ += _timespan;
	fade_out_time_ -= _timespan;
	if(start_challenge_)	//Fade out of MenuScene and in with GameScene
	{
		start_challenge_ = false;
		std::vector<BaseScene_ptr> fo_done_scenes;
		fo_done_scenes.push_back(BaseScene_ptr(new GameScene(challenge_name_)));
		fo_done_scenes.push_back(BaseScene_ptr(new FadeInScene()));
		BaseScene_ptr fo = BaseScene_ptr(new FadeOutScene(fo_done_scenes));
		_new_scenes.push_back(fo);
	}
	if(start_editor_)
	{
		start_editor_ = false;
		std::vector<BaseScene_ptr> fo_done_scenes;
		fo_done_scenes.push_back(BaseScene_ptr(new EditorScene()));
		fo_done_scenes.push_back(BaseScene_ptr(new FadeInScene()));
		BaseScene_ptr fo = BaseScene_ptr(new FadeOutScene(fo_done_scenes));
		_new_scenes.push_back(fo);
	}
	if(exit_game_)
	{
		exit_game_ = false;
		std::vector<BaseScene_ptr> fo_done_scenes;
		BaseScene_ptr fo = BaseScene_ptr(new FadeOutScene());
		_new_scenes.push_back(fo);
	}
}

MenuScene::~MenuScene(void)
{
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Root");
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Start");
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Quit");
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Editor");
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Levels");
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
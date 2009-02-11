#include "StdAfx.h"
#include "MenuScene.h"
#include "GameScene.h"
#include "EditorScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"
#include "Settings.h"
#include "ScoreKeeper.h"
#include <sdl.h>

#include <boost/filesystem.hpp>
#include "SoundManager.h"

bool MenuScene::cbChallengeSelectionChanged(const CEGUI::EventArgs& e)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Listbox* pWndChallengeList = (CEGUI::Listbox*)wmgr.getWindow("Menu/LevelsList");
	CEGUI::PushButton* pBtnStart = (CEGUI::PushButton*)wmgr.getWindow("Menu/Start");
	if(pWndChallengeList->getSelectedCount() == 1)
		pBtnStart->setEnabled(true);
	else
		pBtnStart->setEnabled(false);
	return true;
}

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
	fade_out_time_ = FadeOutScene::FOTime * 0.9f;
	fading_out_ = true;

	SoundManager::Instance().PlaySample("load_challenge.wav");
	 
	return true;
}

bool MenuScene::StartEditor(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;
	
	start_editor_ = true;
	lock_gui_ = true;
	fade_out_time_ = FadeOutScene::FOTime * 0.9f;
	fading_out_ = true;

	return true;
}

bool MenuScene::cbSettingsOK(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pWndSettings = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Menu/Settings");
	pWndSettings->setVisible(false);
	pWndSettings->setModalState(false);

	CEGUI::Checkbox* pCmbFullscreen = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("Menu/Settings/Fullscreen");
	Settings::Instance().SetFullscreen(pCmbFullscreen->isSelected());

	CEGUI::Combobox* pCmboResolution = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("Menu/Settings/Resolutions");
	if(pCmboResolution->getSelectedItem() != NULL)
	{
		int x;
		int y;
		std::string resolution_string = pCmboResolution->getSelectedItem()->getText().c_str();
		size_t x_it = resolution_string.find_first_of("x");
		assert(x_it != resolution_string.npos); // Should never fire as strings hard coded

		std::string xs = resolution_string.substr(0, x_it);
		std::string ys = resolution_string.substr(x_it+1);
		x = boost::lexical_cast<int, std::string>(xs);
		y = boost::lexical_cast<int, std::string>(ys);
		Settings::Instance().SetResolution(Vector2<int>(x, y));
	}


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
	fade_out_time_ = FadeOutScene::FOTime * 0.9f;
	fading_out_ = true;

	return true;
}

bool MenuScene::cbMouseMoveOverChallenges(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;
	CEGUI::MouseEventArgs me = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Listbox* lb = (CEGUI::Listbox*)wmgr.getWindow("Menu/LevelsList");
	//relativeToAbsolute(screenToWindow(me.position)); 
	CEGUI::Vector2 v =  CEGUI::Vector2(me.position.d_x - lb->getPixelRect().d_left, me.position.d_y - lb->getPixelRect().d_top);

	float y = lb->getListRenderArea().getPosition().d_y;
	for(size_t i = 0; i < lb->getItemCount(); i++)
	{
		CEGUI::ListboxItem* lbi = lb->getListboxItemFromIndex(i);
		y += lbi->getPixelSize().d_height;
		if(v.d_y < y)
		{
			lb->setTooltipText(lbi->getTooltipText());
			CEGUI::System::getSingleton().getDefaultTooltip()->positionSelf();
			return true;
		}
	}
	lb->setTooltipText("Hover over a level to see your record!");
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
	pBtnQuit->setSize( CEGUI::UVector2( CEGUI::UDim( 0.15f , 0 ), CEGUI::UDim( 0.05f, 0 ) ) );
	pBtnQuit->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.02f, 0), CEGUI::UDim( 0.9f, 0 ) ) );
	pBtnQuit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::ExitGame, this));
	pBtnQuit->setText( "Quit" );
	pBtnQuit->setTooltipText("Are you leaving so soon?");
	myRoot->addChildWindow(pBtnQuit);

	CEGUI::PushButton* pBtnStart = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/Start");
	pBtnStart->setSize( CEGUI::UVector2( CEGUI::UDim( 0.15f, 0 ), CEGUI::UDim( 0.05f, 0 ) ) );
	pBtnStart->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.02f, 0), CEGUI::UDim( 0.05f, 0 ) ) );
	pBtnStart->setText( "Start Game" );
	pBtnStart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::StartChallenge, this));
	pBtnStart->setEnabled(false);
	pBtnStart->setTooltipText("Start the selected challenge");
	myRoot->addChildWindow(pBtnStart);

	CEGUI::PushButton* pBtnEditor = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/Editor");
	pBtnEditor->setSize( CEGUI::UVector2( CEGUI::UDim( 0.15f, 0 ), CEGUI::UDim( 0.05f, 0 ) ) );
	pBtnEditor->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.02f, 0), CEGUI::UDim( 0.15f, 0 ) ) );
	pBtnEditor->setText( "Ship Editor" );
	pBtnEditor->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::StartEditor, this));
	pBtnEditor->setTooltipText("Create your own ships!");
	myRoot->addChildWindow(pBtnEditor);

	CEGUI::PushButton* pBtnSettings = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Menu/SettingsBtn");
	pBtnSettings->setSize( CEGUI::UVector2( CEGUI::UDim( 0.15f, 0 ), CEGUI::UDim( 0.05f, 0 ) ) );
	pBtnSettings->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.02f, 0), CEGUI::UDim( 0.25f, 0 ) ) );
	pBtnSettings->setText( "Settings" );
	pBtnSettings->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuScene::cbSettings, this));
	pBtnSettings->setTooltipText("Change resolution and other settings");
	myRoot->addChildWindow(pBtnSettings);

	CEGUI::Window* pWndChallenges = wmgr.createWindow( "TaharezLook/FrameWindow", "Menu/Levels" );
	pWndChallenges->setPosition(CEGUI::UVector2(CEGUI::UDim(0.2f, 0),CEGUI::UDim(0.02f,0)));
	pWndChallenges->setSize(CEGUI::UVector2(CEGUI::UDim(0.75f,0), CEGUI::UDim(0.96f,0)));
	pWndChallenges->setProperty("SizingEnabled", "False");
	pWndChallenges->setProperty("CloseButtonEnabled", "False");
	pWndChallenges->setProperty("DragMovingEnabled", "False");
	pWndChallenges->setText("Challenges");
	myRoot->addChildWindow(pWndChallenges);

	CEGUI::Listbox* pWndChallengeList = (CEGUI::Listbox*)wmgr.createWindow( "TaharezLook/Listbox", "Menu/LevelsList" );
	pWndChallengeList->setPosition(CEGUI::UVector2(CEGUI::UDim(0.02f,0),CEGUI::UDim(0.00f, 40)));
	pWndChallengeList->setSize(CEGUI::UVector2(CEGUI::UDim(0.95f,0), CEGUI::UDim(0.975f, -40)));
	pWndChallengeList->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&MenuScene::cbChallengeSelectionChanged, this));
	pWndChallenges->addChildWindow(pWndChallengeList);

	pWndChallengeList->setTooltipText("Select a challenge");
	pWndChallengeList->setInheritsTooltipText(true);
	pWndChallengeList->subscribeEvent(CEGUI::Listbox::EventMouseMove, CEGUI::Event::Subscriber(&MenuScene::cbMouseMoveOverChallenges, this));
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
				std::string challenge_name = boost::filesystem::basename(itr->path());
				if(challenge_name != "EditorTemp")
				{
					CEGUI::ListboxTextItem* lbi = new CEGUI::ListboxTextItem(challenge_name);
					lbi->setSelectionBrushImage(sel_img);
					ChallengeVariantRecord record = Scorekeeper::Instance().QueryProgress(challenge_name);
					lbi->setTooltipText(challenge_name + "\n" +
										"Victories: " + boost::lexical_cast<std::string, int>(record.victories) +
										"\nDefeats: " + boost::lexical_cast<std::string, int>(record.defeats));
					if(record.victories == 0)
						lbi->setTextColours(CEGUI::colour(255,255,127));
					pWndChallengeList->addItem(lbi);
					
				}
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
	
		CEGUI::Combobox* pCmbResolution = (CEGUI::Combobox*)wmgr.createWindow("TaharezLook/Combobox", "Menu/Settings/Resolutions");
		pCmbResolution->setSize(CEGUI::UVector2(CEGUI::UDim(1, -20), CEGUI::UDim(0.5, 30)));
		pCmbResolution->setReadOnly(true);
		pCmbResolution->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 10), CEGUI::UDim(0, 30)));
		
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("320x240"));
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("640x480"));
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("800x600"));
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("1024x768"));
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("1280x1024"));
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("1600x1200"));
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("800x480"));
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("1024x600"));
		pCmbResolution->addItem(new CEGUI::ListboxTextItem("1280x768"));
		
		pWndSettings->addChildWindow(pCmbResolution);

		CEGUI::Checkbox* pCmbCheckbox = (CEGUI::Checkbox*)wmgr.createWindow("TaharezLook/Checkbox", "Menu/Settings/Fullscreen");

		pCmbCheckbox->setSize(CEGUI::UVector2(CEGUI::UDim(1, -20), CEGUI::UDim(0, 30)));
		pCmbCheckbox->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 10), CEGUI::UDim(0, 70)));
		pCmbCheckbox->setText("Fullscreen");
		pCmbCheckbox->setSelected(Settings::Instance().GetFullscreen());
		pWndSettings->addChildWindow(pCmbCheckbox);
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
	//CEGUI::WindowManager::getSingleton().destroyAllWindows();
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Root");
	/*CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Start");
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Quit");
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Editor");
	CEGUI::WindowManager::getSingleton().destroyWindow("Menu/Levels");*/
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
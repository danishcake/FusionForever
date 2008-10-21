#include "StdAfx.h"
#include "EditorScene.h"
#include "MenuScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"
#include "EditorGame.h"
#include <sdl.h>

#include "SquareCore.h"
#include "TinyCore.h"

#include "Blaster.h"
#include "HeatBeamGun.h"
#include "HomingMissileLauncher.h"
#include "Swarmer.h"
#include "LuaSection.h"

#include "RotatingAI.h"

#include <boost/filesystem.hpp>

bool EditorScene::cbReturnToMenu(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;
	
	return_to_menu_ = true;
	lock_gui_ = true;
	fade_out_time_ = 1.0f;
	fading_out_ = true;

	return true;
}

bool EditorScene::cbSave(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;

	this->game_->GetCore()->SaveToXML("temp_ship.xmlShip");
	return true;
}

bool EditorScene::cbSetCoreToSquareCore(const CEGUI::EventArgs& e)
{
	this->game_->SetCore(new SquareCore(new RotatingAI(0.05f)));
	selection_ = static_cast<Section_ptr>(this->game_->GetCore());
	selection_->SetFlashing();
	return true;
}

bool EditorScene::cbSetCoreToTinyCore(const CEGUI::EventArgs& e)
{
	this->game_->SetCore(new TinyCore(new RotatingAI(0.05f)));
	selection_ = static_cast<Section_ptr>(this->game_->GetCore());
	selection_->SetFlashing();
	return true;
}

bool EditorScene::cbAddBlaster(const CEGUI::EventArgs& e)
{
	if(selection_ != NULL)
	{
		Section_ptr section = new Blaster();
		selection_->AddChild(section);
		selection_->SetNotFlashing();
		selection_ = section;
		selection_->SetFlashing();
	}
	return true;
}
bool EditorScene::cbAddHeatBeam(const CEGUI::EventArgs& e)
{
	if(selection_ != NULL)
	{
		Section_ptr section = new HeatBeamGun();
		selection_->AddChild(section);
		selection_->SetNotFlashing();
		selection_ = section;
		selection_->SetFlashing();
	}
	return true;
}
bool EditorScene::cbAddHomingMissileLauncher(const CEGUI::EventArgs& e)
{
	if(selection_ != NULL)
	{
		Section_ptr section = new HomingMissileLauncher();
		selection_->AddChild(section);
		selection_->SetNotFlashing();
		selection_ = section;
		selection_->SetFlashing();
	}
	return true;
}
bool EditorScene::cbAddSwarmer(const CEGUI::EventArgs& e)
{
	if(selection_ != NULL)
	{
		Section_ptr section = new Swarmer();
		selection_->AddChild(section);
		selection_->SetNotFlashing();
		selection_ = section;
		selection_->SetFlashing();
	}
	return true;
}

bool EditorScene::cbAddLuaSection(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& we = 	static_cast<const CEGUI::WindowEventArgs&>(e);
	if(selection_ != NULL)
	{
		Section_ptr section = LuaSection::CreateLuaSection(we.window->getName().c_str(), game_->GetLua());
		if(section != NULL)
		{
			selection_->SetNotFlashing();
			selection_->AddChild(section);
			selection_ = section;
			selection_->SetFlashing();
		} else
		{
			Logger::LogError("Unable to load lua section in editor");
			Logger::LogError(we.window->getName().c_str());
		}
	}
	return true;
}

bool EditorScene::cbChangeOrientation(const CEGUI::EventArgs& e)
{
	CEGUI::Slider* slider = static_cast<CEGUI::Slider*>(CEGUI::WindowManager::getSingleton().getWindow("Edit/Properties/Orientation"));
	if(selection_ != NULL && selection_!= game_->GetCore())
		selection_->SetAngle(slider->getCurrentValue()-180.0f);
	return true;
}

bool EditorScene::cbChangeAngle(const CEGUI::EventArgs& e)
{
	CEGUI::Slider* slider = static_cast<CEGUI::Slider*>(CEGUI::WindowManager::getSingleton().getWindow("Edit/Properties/Angle"));
	if(selection_ != NULL && selection_!= game_->GetCore())
	{
		float distance = selection_->GetPosition().length();
		float angle = static_cast<float>(DEG2RAD((slider->getCurrentValue() - 180)));
		selection_->SetPosition(Vector3f(sin(angle), cos(angle), 0) * distance);
	}
	return true;
}

bool EditorScene::cbChangeDistance(const CEGUI::EventArgs& e)
{
	CEGUI::Slider* slider = static_cast<CEGUI::Slider*>(CEGUI::WindowManager::getSingleton().getWindow("Edit/Properties/Distance"));
	if(selection_ != NULL && selection_!= game_->GetCore())
	{
		float distance = slider->getCurrentValue();
		float angle = atan2f(selection_->GetPosition().x, selection_->GetPosition().y);
		selection_->SetPosition(Vector3f(sin(angle), cos(angle), 0) * distance);
	}
	return true;
}

bool EditorScene::cbBackgroundClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs we = static_cast<const CEGUI::MouseEventArgs&>(e);
	Logger::Log(std::string("Clicked at ") + boost::lexical_cast<std::string, float>(we.position.d_x) + std::string(",") + boost::lexical_cast<std::string, float>(we.position.d_y));
	Vector3f world_click = Camera::Instance().ScreenToWorld(Vector3f(we.position.d_x, we.position.d_y, 0));
	Logger::Log(std::string("In world coords that is ") + boost::lexical_cast<std::string, float>(world_click.x) + std::string(",") + boost::lexical_cast<std::string, float>(world_click.y));
	Section* clicked_section = game_->GetAtMouseCoord(world_click);
	if(clicked_section != NULL)
	{
		if(selection_ != NULL)
			selection_->SetNotFlashing();
		selection_ = clicked_section;
			selection_->SetFlashing();
		CEGUI::Slider* slider_angle = static_cast<CEGUI::Slider*>(CEGUI::WindowManager::getSingleton().getWindow("Edit/Properties/Angle"));
		CEGUI::Slider* slider_distance = static_cast<CEGUI::Slider*>(CEGUI::WindowManager::getSingleton().getWindow("Edit/Properties/Distance"));
		CEGUI::Slider* slider_orientation = static_cast<CEGUI::Slider*>(CEGUI::WindowManager::getSingleton().getWindow("Edit/Properties/Orientation"));

		float angle = RAD2DEG(atan2f(selection_->GetPosition().x, selection_->GetPosition().y));
		float distance = selection_->GetPosition().length();
		float orientation = selection_->GetAngle();
		slider_angle->setCurrentValue(angle+180);
		slider_distance->setCurrentValue(distance);
		slider_orientation->setCurrentValue(orientation+180);

	}
	return true;
}


EditorScene::EditorScene(void)
{
	fading_out_ = false;
	fade_out_time_ = 0;
	lock_gui_ = false;
	return_to_menu_ = false;

	sum_time_ = 0;

	game_ = new EditorGame();
	selection_ = game_->GetCore();

	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* myRoot = wmgr.createWindow( "DefaultWindow", "Edit/Root" );

	CEGUI::Window* pWndClickArea = wmgr.createWindow("DefaultWindow", "Edit/ClickArea");
	pWndClickArea->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, 130 ), CEGUI::UDim( 0, 0 ) ) );
	pWndClickArea->setSize(CEGUI::UVector2( CEGUI::UDim( 1, -130 ), CEGUI::UDim( 1, -120 ) ) );
	pWndClickArea->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&EditorScene::cbBackgroundClick, this));
	myRoot->addChildWindow(pWndClickArea);

	CEGUI::PushButton* pBtnQuit = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/QuitToMenu");
	pBtnQuit->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 55 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnQuit->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 75), CEGUI::UDim( 0, 10 ) ) );
	pBtnQuit->setText( "Quit" );
	pBtnQuit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbReturnToMenu, this));
	myRoot->addChildWindow(pBtnQuit);

	CEGUI::PushButton* pBtnStart = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/Try");
	pBtnStart->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 55 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnStart->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 0.0f, 10 ) ) );
	pBtnStart->setText( "Try it!" );
	myRoot->addChildWindow(pBtnStart);

	CEGUI::PushButton* pBtnSave = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/Save");
	pBtnSave->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 55 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnSave->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 140), CEGUI::UDim( 0.0f, 10 ) ) );
	pBtnSave->setText( "Save" );
	pBtnSave->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&EditorScene::cbSave, this));
	myRoot->addChildWindow(pBtnSave);

	CEGUI::PushButton* pBtnAdd = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/Add");
	pBtnAdd->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 45 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnAdd->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -110), CEGUI::UDim( 1, -75 ) ) );
	pBtnAdd->setText( "Add" );
	myRoot->addChildWindow(pBtnAdd);

	CEGUI::PushButton* pBtnDelete = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/Delete");
	pBtnDelete->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 45 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnDelete->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -60), CEGUI::UDim( 1, -75 ) ) );
	pBtnDelete->setText( "Del" );
	myRoot->addChildWindow(pBtnDelete);
	
	CEGUI::PushButton* pBtnDeleteTree = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/DeleteTree");
	pBtnDeleteTree->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 45 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnDeleteTree->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -60), CEGUI::UDim( 1, -110 ) ) );
	pBtnDeleteTree->setText( "Deltree" );
	myRoot->addChildWindow(pBtnDeleteTree);

	CEGUI::GroupBox* pTreeFrame = (CEGUI::GroupBox*)wmgr.createWindow("TaharezLook/GroupBox", "Edit/Frame");
	pTreeFrame->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 120 ), CEGUI::UDim( 1, -190) ) );
	pTreeFrame->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 0.0f, 50 ) ) );
	pTreeFrame->setText("Ship");
	myRoot->addChildWindow(pTreeFrame);

	CEGUI::Tree* pShip = (CEGUI::Tree*)wmgr.createWindow("TaharezLook/Tree", "Edit/Ship");
	pShip->setSize(     CEGUI::UVector2( CEGUI::UDim( 1, 0), CEGUI::UDim( 1   , 0)));
	pShip->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 0), CEGUI::UDim( 0.0f, 0)));
	CEGUI::TreeItem* pItem = new CEGUI::TreeItem("Item A");
	pShip->addItem(pItem);
	pTreeFrame->addChildWindow(pShip);

	CEGUI::GroupBox* pProperties =(CEGUI::GroupBox*)wmgr.createWindow("TaharezLook/GroupBox", "Edit/Properties");
	pProperties->setSize(CEGUI::UVector2( CEGUI::UDim( 0, 120 ), CEGUI::UDim( 0, 120 ) ) );
	pProperties->setPosition(  CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 1.0f, -130)));
	pProperties->setText("Properties");
	myRoot->addChildWindow(pProperties);

	CEGUI::Slider* pOrientation = (CEGUI::Slider*)wmgr.createWindow("TaharezLook/Slider", "Edit/Properties/Orientation");
	pOrientation->setSize(CEGUI::UVector2( CEGUI::UDim( 0, 15 ), CEGUI::UDim( 0, 80 ) ) );
	pOrientation->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, 10 ), CEGUI::UDim( 0, 10 ) ) );
	pOrientation->setMaxValue(360);
	pOrientation->setCurrentValue(180);
	pOrientation->setClickStep(15);
	pOrientation->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&EditorScene::cbChangeOrientation, this));
	pProperties->addChildWindow(pOrientation);

	CEGUI::Slider* pAngle = (CEGUI::Slider*)wmgr.createWindow("TaharezLook/Slider", "Edit/Properties/Angle");
	pAngle->setSize(CEGUI::UVector2( CEGUI::UDim( 0, 15 ), CEGUI::UDim( 0, 80 ) ) );
	pAngle->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, 35 ), CEGUI::UDim( 0, 10 ) ) );
	pAngle->setMaxValue(360);
	pAngle->setCurrentValue(180);
	pAngle->setClickStep(15);
	pAngle->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&EditorScene::cbChangeAngle, this));
	pProperties->addChildWindow(pAngle);

	CEGUI::Slider* pDistance = (CEGUI::Slider*)wmgr.createWindow("TaharezLook/Slider", "Edit/Properties/Distance");
	pDistance->setSize(CEGUI::UVector2( CEGUI::UDim( 0, 15 ), CEGUI::UDim( 0, 80 ) ) );
	pDistance->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, 60 ), CEGUI::UDim( 0, 10 ) ) );
	pDistance->setMaxValue(20);
	pDistance->setCurrentValue(5);
	pDistance->setClickStep(1);
	pDistance->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&EditorScene::cbChangeDistance, this));
	pProperties->addChildWindow(pDistance);



	CEGUI::TabControl* pPalette = (CEGUI::TabControl*)wmgr.createWindow("TaharezLook/TabControl", "Edit/Palette");
		pPalette->setSize(CEGUI::UVector2( CEGUI::UDim( 1, -260 ), CEGUI::UDim( 0, 110 ) ) );
		pPalette->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 140), CEGUI::UDim( 1.0f, -120 ) ) );
		
		CEGUI::DefaultWindow* pTabCores = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/TabContentPane");
			pTabCores->setProperty("EnableBottom","1");
			pTabCores->setText("Cores");
			{
			int width = 2;
			int height = 1;
			CEGUI::PushButton* pBtnSetCoreToSquareCore = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button");
				pBtnSetCoreToSquareCore->setText("Square");
				pBtnSetCoreToSquareCore->setSize(    CEGUI::UVector2( CEGUI::UDim( 0, 60 ),    CEGUI::UDim( 0, 20 ) ) );
				pBtnSetCoreToSquareCore->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, width ), CEGUI::UDim( 0, height ) ) );
				pBtnSetCoreToSquareCore->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbSetCoreToSquareCore, this));
				width += 61;
			pTabCores->addChildWindow(pBtnSetCoreToSquareCore);
			CEGUI::PushButton* pBtnSetCoreToTinyCore = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button");
				pBtnSetCoreToTinyCore->setText("Tiny");
				pBtnSetCoreToTinyCore->setSize(    CEGUI::UVector2( CEGUI::UDim( 0, 60 ),    CEGUI::UDim( 0, 20 ) ) );
				pBtnSetCoreToTinyCore->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, width ), CEGUI::UDim( 0, height ) ) );
				pBtnSetCoreToTinyCore->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbSetCoreToTinyCore, this));
				width += 61;
			pTabCores->addChildWindow(pBtnSetCoreToTinyCore);
			}
		pPalette->addTab(pTabCores);
		CEGUI::DefaultWindow* pTabWeapons = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/TabContentPane");
			pTabWeapons->setProperty("EnableBottom","1");
			pTabWeapons->setText("Weapons");
			{
			int width = 2;
			int height = 1;
			CEGUI::PushButton* pBtnAddBlaster = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button");
				pBtnAddBlaster->setText("Blaster");
				pBtnAddBlaster->setSize(    CEGUI::UVector2( CEGUI::UDim( 0, 60 ),    CEGUI::UDim( 0, 20 ) ) );
				pBtnAddBlaster->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, width ), CEGUI::UDim( 0, height ) ) );
				pBtnAddBlaster->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbAddBlaster, this));
				width += 61;
			pTabWeapons->addChildWindow(pBtnAddBlaster);
			CEGUI::PushButton* pBtnAddHeatBeam = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button");
				pBtnAddHeatBeam->setText("HeatBeam");
				pBtnAddHeatBeam->setSize(    CEGUI::UVector2( CEGUI::UDim( 0, 60 ),    CEGUI::UDim( 0, 20 ) ) );
				pBtnAddHeatBeam->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, width ), CEGUI::UDim( 0, height ) ) );
				pBtnAddHeatBeam->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbAddHeatBeam, this));
				width += 61;
			pTabWeapons->addChildWindow(pBtnAddHeatBeam);
			CEGUI::PushButton* pBtnAddHomingMissileLauncher = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button");
				pBtnAddHomingMissileLauncher->setText("HomingMissile");
				pBtnAddHomingMissileLauncher->setSize(    CEGUI::UVector2( CEGUI::UDim( 0, 60 ),    CEGUI::UDim( 0, 20 ) ) );
				pBtnAddHomingMissileLauncher->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, width ), CEGUI::UDim( 0, height ) ) );
				pBtnAddHomingMissileLauncher->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbAddHomingMissileLauncher, this));
				width += 61;
			pTabWeapons->addChildWindow(pBtnAddHomingMissileLauncher);
			CEGUI::PushButton* pBtnAddSwarmer = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button");
				pBtnAddSwarmer->setText("Swarmer");
				pBtnAddSwarmer->setSize(    CEGUI::UVector2( CEGUI::UDim( 0, 60 ),    CEGUI::UDim( 0, 20 ) ) );
				pBtnAddSwarmer->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, width ), CEGUI::UDim( 0, height ) ) );
				pBtnAddSwarmer->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbAddSwarmer, this));
				width += 61;
			pTabWeapons->addChildWindow(pBtnAddSwarmer);
			}
		pPalette->addTab(pTabWeapons);

		CEGUI::DefaultWindow* pTabLua = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/TabContentPane");
			pTabLua->setProperty("EnableBottom","1");
			pTabLua->setText("Lua Sections");
			{
			int width = 2;
			int height = 1;
			boost::filesystem::directory_iterator end_itr;	
			for(boost::filesystem::directory_iterator itr = boost::filesystem::directory_iterator("./Scripts/Sections");
				itr != end_itr;
				++itr)
			{
				if(boost::filesystem::is_regular((itr->status())))
				{
					std::string ext = boost::filesystem::extension(*itr);
					if(ext == ".luaSection")
					{
						std::string filename = boost::filesystem::basename(itr->path());
						CEGUI::PushButton* pBtnLuaSection = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button", filename);
						pBtnLuaSection->setText(filename);
							pBtnLuaSection->setSize(    CEGUI::UVector2( CEGUI::UDim( 0, 60 ),    CEGUI::UDim( 0, 20 ) ) );
							pBtnLuaSection->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, width ), CEGUI::UDim( 0, height ) ) );
							pBtnLuaSection->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbAddLuaSection, this));
							width += 61;
						pTabLua->addChildWindow(pBtnLuaSection);
					}
				}
			}
			}
		pPalette->addTab(pTabLua);
	myRoot->addChildWindow(pPalette);
}

EditorScene::~EditorScene(void)
{
	CEGUI::WindowManager::getSingleton().destroyWindow("Edit/Root");
	CEGUI::WindowManager::getSingleton().destroyWindow("Edit/QuitToMenu");
	CEGUI::WindowManager::getSingleton().destroyWindow("Edit/Try");
	delete game_;
}

void EditorScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	if(sum_time_ == 0)
		CEGUI::System::getSingleton().setGUISheet( CEGUI::WindowManager::getSingleton().getWindow("Edit/Root"));
	sum_time_ += _timespan;
	fade_out_time_ -= _timespan;

	game_->Tick(_timespan);

	if(selection_ != NULL)
		selection_->SetFlashing();

	if(return_to_menu_)
	{
		return_to_menu_ = false;
		std::vector<BaseScene_ptr> fo_done_scenes;
		fo_done_scenes.push_back(BaseScene_ptr(new MenuScene()));
		fo_done_scenes.push_back(BaseScene_ptr(new FadeInScene()));
		BaseScene_ptr fo = BaseScene_ptr(new FadeOutScene(fo_done_scenes));
		_new_scenes.push_back(fo);
	}
}


void EditorScene::Draw()
{
	game_->Draw();
}

bool EditorScene::IsRemovable()
{
	return (fading_out_ && (fade_out_time_ < 0));
}

bool EditorScene::IsRoot()
{
	return true;
}


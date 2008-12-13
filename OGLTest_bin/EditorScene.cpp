#include "StdAfx.h"
#include "EditorScene.h"
#include "MenuScene.h"
#include "FadeOutScene.h"
#include "FadeInScene.h"
#include "EditorGame.h"
#include "Camera.h"
#include <sdl.h>

#include "XMLCore.h"

#include "Blaster.h"
#include "HeatBeamGun.h"
#include "HomingMissileLauncher.h"
#include "Swarmer.h"
#include "ChainGun.h"
#include "PlasmaArtillery.h"
#include "SpinningJoint.h"
#include "JointAngles.h"
#include "JointTracker.h"
#include "XmlSection.h"
#include "Property.h"
#include "RotatingAI.h"


#include <boost/filesystem.hpp>

bool EditorScene::cbAddSection(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& we = 	static_cast<const CEGUI::WindowEventArgs&>(e);
	if(selection_ != NULL)
	{
		Section_ptr section = NULL;
		section = ListAdder::GetSection(we.window->getText().c_str());
		if(!section)
			XMLSection::CreateXMLSection(we.window->getText().c_str());
		if(section != NULL)
		{
			selection_->AddChild(section);
			SetSelected(section);
			
		} else
		{
			Logger::LogError("Unable to load section in editor from either XML or hardcoded list");
			Logger::LogError(we.window->getText().c_str());
		}
	}
	return true;
}

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
	CEGUI::Window* pWndSave = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Edit/SaveDialogue");
	CEGUI::Listbox* pLbShips = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("Edit/SaveDialogue/Shiplist");
	CEGUI::Editbox* pTbFilename = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("Edit/SaveDialogue/Filename");
	pWndSave->setVisible(true);
	pWndSave->setModalState(true);

	pLbShips->resetList();

	const CEGUI::Image* sel_img = &CEGUI::ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush");

	boost::filesystem::directory_iterator end_itr;	
	for(boost::filesystem::directory_iterator itr = boost::filesystem::directory_iterator("./Scripts/Ships");
		itr != end_itr;
		++itr)
	{
		if(boost::filesystem::is_regular((itr->status())))
		{
			std::string ext = boost::filesystem::extension(*itr);
			if(ext == ".xmlShip")
			{
				CEGUI::ListboxItem* lbi = new CEGUI::ListboxTextItem(boost::filesystem::basename(itr->path()));
				lbi->setSelectionBrushImage(sel_img);
				pLbShips->addItem(lbi);
			}
		}
	}

	pTbFilename->setText(filename_);

	return true;
}

bool EditorScene::cbSaveDialogueListSelected(const CEGUI::EventArgs& e)
{
	CEGUI::Listbox* pLbShips = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("Edit/SaveDialogue/Shiplist");
	CEGUI::Editbox* pTbFilename = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("Edit/SaveDialogue/Filename");
	CEGUI::ListboxItem * selectedItem = pLbShips->getFirstSelectedItem();
	pTbFilename->setText(selectedItem->getText());
	
	return true;
}

bool EditorScene::cbSaveDialogueSave(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pWndSave = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Edit/SaveDialogue");
	pWndSave->setVisible(false);
	pWndSave->setModalState(false);
	CEGUI::Editbox* pTbFilename = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("Edit/SaveDialogue/Filename");

	game_->GetCore()->SaveToXML(std::string(pTbFilename->getText().c_str()) + std::string(".xmlShip"));
	filename_ = pTbFilename->getText().c_str();
	return true;
}

bool EditorScene::cbSaveDialogueCancel(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pWndSave = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Edit/SaveDialogue");
	pWndSave->setVisible(false);
	pWndSave->setModalState(false);
	return true;
}

bool EditorScene::cbLoad(const CEGUI::EventArgs& e)
{
	if(lock_gui_)
		return true;
	CEGUI::Window* pWndLoad = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Edit/LoadDialogue");
	CEGUI::Listbox* pLbShips = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("Edit/LoadDialogue/Shiplist");
	pWndLoad->setVisible(true);
	pWndLoad->setModalState(true);

	pLbShips->resetList();
	
	const CEGUI::Image* sel_img = &CEGUI::ImagesetManager::getSingleton().getImageset("TaharezLook")->getImage("MultiListSelectionBrush");

	boost::filesystem::directory_iterator end_itr;	
	for(boost::filesystem::directory_iterator itr = boost::filesystem::directory_iterator("./Scripts/Ships");
		itr != end_itr;
		++itr)
	{
		if(boost::filesystem::is_regular((itr->status())))
		{
			std::string ext = boost::filesystem::extension(*itr);
			if(ext == ".xmlShip")
			{
				CEGUI::ListboxItem* lbi = new CEGUI::ListboxTextItem(boost::filesystem::basename(itr->path()));
				lbi->setSelectionBrushImage(sel_img);
				pLbShips->addItem(lbi);
			}
		}
	}

	return true;
}

bool EditorScene::cbLoadDialogueLoad(const CEGUI::EventArgs& e)
{
	CEGUI::Listbox* pLbShips = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("Edit/LoadDialogue/Shiplist");
	CEGUI::Window* pWndLoad = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Edit/LoadDialogue");
	if(pLbShips->getSelectedCount() == 1)
	{
		std::string filename = pLbShips->getFirstSelectedItem()->getText().c_str();
		Core_ptr loaded_core = Core::CreateCore(filename);
		if(loaded_core)
		{
			loaded_core->OverrideAI(new RotatingAI(0));
			game_->LoadCore(loaded_core);
			SetSelected(loaded_core);
			filename_ = filename;
		} else
		{
			Logger::Instance() << "Unable to load core \"" << filename << "\n";
		}
		pWndLoad->setVisible(false);
		pWndLoad->setModalState(false);
	}
	return true;
}

bool EditorScene::cbLoadDialogueCancel(const CEGUI::EventArgs& e)
{
	CEGUI::Window* pWndLoad = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow("Edit/LoadDialogue");
	pWndLoad->setVisible(false);
	pWndLoad->setModalState(false);
	return true;
}

bool EditorScene::cbDelete(const CEGUI::EventArgs& e)
{
	if(selection_ != NULL && !selection_->IsCore())
	{
		Section_ptr parent = selection_->GetParent();
		std::vector<Section_ptr> detached = selection_->DetachChildren();
		detached.erase(std::remove( detached.begin(), detached.end(), selection_), detached.end());

		std::vector<Section_ptr> parent_detached = selection_->GetParent()->DetachChildren();
		parent_detached.erase(std::remove( parent_detached.begin(), parent_detached.end(), selection_), parent_detached.end());
		delete selection_;

		parent->AttachChildren(parent_detached);
		parent->AttachChildren(detached);


		parent->AttachChildren(detached);
		SetSelected(parent);
	}
	return true;
}

bool EditorScene::cbDeleteTree(const CEGUI::EventArgs& e)
{
	if(selection_ != NULL && !selection_->IsCore())
	{
		Section_ptr parent = selection_->GetParent();
		std::vector<Section_ptr> detached = selection_->GetParent()->DetachChildren();
		detached.erase(std::remove( detached.begin(), detached.end(), selection_), detached.end());
		delete selection_;
		parent->AttachChildren(detached);

		SetSelected(parent);
	}
	return true;
}

bool EditorScene::cbSetCoreToXMLCore(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& we = 	static_cast<const CEGUI::WindowEventArgs&>(e);
	if(selection_ != NULL)
	{
		Core_ptr core = XMLCore::CreateXMLCore(we.window->getText().c_str());
		if(core != NULL)
		{
			core->OverrideAI(new RotatingAI(0));
			game_->SetCore(core);
			SetSelected(static_cast<Section_ptr>(this->game_->GetCore()));
		} else
		{
			Logger::LogError("Unable to load XML section in editor");
			Logger::LogError(we.window->getText().c_str());
		}
	}
	return true;
}


bool EditorScene::cbBackgroundClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs we = static_cast<const CEGUI::MouseEventArgs&>(e);

	if(we.button == CEGUI::LeftButton)
	{
		Vector3f world_click = Camera::Instance().ScreenToWorld(Vector3f(we.position.d_x, we.position.d_y, 0));
		Section* clicked_section = game_->GetAtMouseCoord(world_click);

		static std::list<Section_ptr> ltv_clicked_sections;
		static int item_index = 0;
		std::vector<Section_ptr> clicked_sections = game_->GetAllAtMouseCoord(world_click);
		if(clicked_sections.size() > 0)
		{
			//If the list is the same as last time then chose the next item in it.
			if(std::equal(ltv_clicked_sections.begin(), ltv_clicked_sections.end(), clicked_sections.begin()))
			{
				item_index = (item_index + 1)% static_cast<int>(clicked_sections.size()); //Move to the next item in the list
			} else
			{
				item_index = 0; //Move to the first item in the list
			}
			SetSelected(clicked_sections[item_index]);
			move_first_tick = true;
		}
	} else if(we.button == CEGUI::MiddleButton)
	{
		Camera::Instance().SetCentre(0,0,CameraLevel::Human);
	}
	return true;
}

bool EditorScene::cbBackgroundMove(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs we = static_cast<const CEGUI::MouseEventArgs&>(e);

	if(move_first_tick)
	{
		old_position = we.position;
		move_first_tick = false;
	}
	CEGUI::Point mouse_move;
	if(drag_mode_ != EditorDragMode::NotDragging)
		mouse_move = we.position - old_position;
	else
		mouse_move = CEGUI::Point(0,0);
	
	Vector3f world_move = Camera::Instance().ScreenDeltaToWorldDelta(Vector3f(mouse_move.d_x, mouse_move.d_y, 0.0f));
	Vector3f world_position = Camera::Instance().ScreenDeltaToWorldDelta(Vector3f(we.position.d_x, we.position.d_y, 0.0f));

	old_position = we.position;

	if(selection_ != NULL && !selection_->IsCore()) //Something selected and is not root
	{
		if((we.sysKeys & CEGUI::Shift) != CEGUI::Shift) //Shift = free mode
		{
			accumulated_snap += world_move;

			if(drag_mode_ == EditorDragMode::MoveDrag)
			{
				if(fabsf(accumulated_snap.x) >= 2.5f)
				{
					//Rotate the move vector into the parents space
					Vector3f delta = Vector3f(accumulated_snap.x, 0 , 0);
					float angle = selection_->GetParent()->GetGlobalAngle();
					delta.rotate(0,0, angle);
					Vector3f snap_to = (selection_->GetPosition() + delta).snap(2.5f);
					selection_->SetPosition(snap_to);
					accumulated_snap.x = 0;
				}
				if(fabsf(accumulated_snap.y) >= 2.5f)
				{
					//Rotate the move vector into the parents space
					Vector3f delta = Vector3f(0, accumulated_snap.y , 0);
					delta.rotate(0,0, selection_->GetParent()->GetGlobalAngle());
					Vector3f snap_to = (selection_->GetPosition() + delta).snap(2.5f);
					selection_->SetPosition(snap_to);
					accumulated_snap.y = 0;
				}
			} else if(drag_mode_ == EditorDragMode::RotateDrag)
			{
				//Rotate to face the mouse
				if(fabs(accumulated_snap.x) >= 15)
				{
					float angle = floorf((selection_->GetAngle() + accumulated_snap.x) / 15 + 0.5f) * 15.0f;
					selection_->SetAngle(angle);
					accumulated_snap.x = 0;
				}
			}
		} else
		{
			if(drag_mode_ == EditorDragMode::MoveDrag)
			{
				Vector3f delta = world_move;
				float angle = selection_->GetParent()->GetGlobalAngle();
				delta.rotate(0,0, angle);
				selection_->SetPosition(selection_->GetPosition() + delta);

			} else if(drag_mode_ == EditorDragMode::RotateDrag)
			{
				//Rotate to face the mouse
				selection_->SetAngle(selection_->GetAngle() + mouse_move.d_x);
			}
			accumulated_snap = Vector3f();
		}
	}
	if(drag_mode_ ==EditorDragMode::ScrollDrag)
	{
		Camera::Instance().SetCentre(Camera::Instance().GetCentreX() - world_move.x, Camera::Instance().GetCentreY() - world_move.y, CameraLevel::Human);
	}
	return true;
}

bool EditorScene::cbBackgroundMBD(const CEGUI::EventArgs& e)
{
	Logger::Instance() << "Mouse down\n";
	
	const CEGUI::MouseEventArgs we = static_cast<const CEGUI::MouseEventArgs&>(e);
	if(we.button == CEGUI::LeftButton)
	{
		Vector3f world_click = Camera::Instance().ScreenToWorld(Vector3f(we.position.d_x, we.position.d_y, 0));
		//Section* clicked_section = game_->GetAtMouseCoord(world_click);
		
		Section* clicked_section = NULL; 
		selection_->CheckCollisions(world_click, clicked_section);
		if(clicked_section != NULL && clicked_section == selection_)
		{
			drag_mode_ = EditorDragMode::MoveDrag;
		} else
		{
			drag_mode_ = EditorDragMode::RotateDrag;
		}
	} else if(we.button == CEGUI::MiddleButton)
		drag_mode_ = EditorDragMode::ScrollDrag;

	return true;
}

bool EditorScene::cbBackgroundMBU(const CEGUI::EventArgs& e)
{
	Logger::Instance() << "Mouse up\n";
	drag_mode_ = EditorDragMode::NotDragging;
	move_first_tick = true;
	accumulated_snap = Vector3f();
	return true;
}

bool EditorScene::cbBackgroundMouseLeave(const CEGUI::EventArgs& e)
{
	Logger::Instance() << "Mouse leaving\n";
	drag_mode_ = EditorDragMode::NotDragging;
	move_first_tick = true;
	accumulated_snap = Vector3f();
	return true;
}


void AddItemToTab(CEGUI::Event::Subscriber _subscriber, CEGUI::String _text, CEGUI::Window* _tab, float& _width, float& _height)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton* pBtn = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button");
	pBtn->setText(_text);
	float section_width = pBtn->getFont()->getFormattedTextExtent(pBtn->getText(), CEGUI::Rect(CEGUI::System::getSingleton().getRenderer()->getRect()), CEGUI::LeftAligned);
	pBtn->setSize(CEGUI::UVector2(CEGUI::UDim( 0, section_width ), 
								  CEGUI::UDim( 0, 20 )));
	if(_width + section_width > (_tab->getUnclippedInnerRect().d_right - _tab->getUnclippedInnerRect().d_left - 10))
	{
		_width = 2;
		_height += 25;
	}
	pBtn->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, _width ), CEGUI::UDim( 0, _height ) ) );
	pBtn->subscribeEvent(CEGUI::PushButton::EventClicked, _subscriber);
	_width += section_width + 5;
	_tab->addChildWindow(pBtn);
}

EditorScene::EditorScene(void)
{
	fading_out_ = false;
	fade_out_time_ = 0;
	lock_gui_ = false;
	return_to_menu_ = false;
	drag_mode_ = EditorDragMode::NotDragging;
	move_first_tick = true;

	sum_time_ = 0;

	game_ = new EditorGame();
	selection_ = game_->GetCore();

	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* myRoot = wmgr.createWindow( "DefaultWindow", "Edit/Root" );

	CEGUI::Window* pWndClickArea = wmgr.createWindow("DefaultWindow", "Edit/ClickArea");
	pWndClickArea->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, 130 ), CEGUI::UDim( 0, 0 ) ) );
	pWndClickArea->setSize(CEGUI::UVector2( CEGUI::UDim( 1, -130 ), CEGUI::UDim( 1, -120 ) ) );
	pWndClickArea->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&EditorScene::cbBackgroundClick, this));
	pWndClickArea->subscribeEvent(CEGUI::Window::EventMouseMove,CEGUI::Event::Subscriber(&EditorScene::cbBackgroundMove, this));
	pWndClickArea->subscribeEvent(CEGUI::Window::EventMouseButtonDown,CEGUI::Event::Subscriber(&EditorScene::cbBackgroundMBD, this));
	pWndClickArea->subscribeEvent(CEGUI::Window::EventMouseButtonUp,CEGUI::Event::Subscriber(&EditorScene::cbBackgroundMBU, this));
	pWndClickArea->subscribeEvent(CEGUI::Window::EventMouseLeaves,CEGUI::Event::Subscriber(&EditorScene::cbBackgroundMouseLeave, this));
	
	myRoot->addChildWindow(pWndClickArea);

	CEGUI::PushButton* pBtnSave = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/Save");
	pBtnSave->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 55 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnSave->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 0.0f, 10 ) ) );
	pBtnSave->setText( "Save" );
	pBtnSave->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&EditorScene::cbSave, this));
	myRoot->addChildWindow(pBtnSave);

	CEGUI::PushButton* pBtnLoad = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/Load");
	pBtnLoad->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 55 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnLoad->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 0.0f, 50 ) ) );
	pBtnLoad->setText( "Load" );
	pBtnLoad->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&EditorScene::cbLoad, this));
	myRoot->addChildWindow(pBtnLoad);

	CEGUI::PushButton* pBtnDelete = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/Delete");
	pBtnDelete->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 55 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnDelete->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 0, 90 ) ) );
	pBtnDelete->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&EditorScene::cbDelete, this));
	pBtnDelete->setText( "Del" );
	myRoot->addChildWindow(pBtnDelete);
	
	CEGUI::PushButton* pBtnDeleteTree = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/DeleteTree");
	pBtnDeleteTree->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 55 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnDeleteTree->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 0, 130 ) ) );
	pBtnDeleteTree->subscribeEvent(CEGUI::Window::EventMouseClick,CEGUI::Event::Subscriber(&EditorScene::cbDeleteTree, this));
	pBtnDeleteTree->setText( "Deltree" );
	myRoot->addChildWindow(pBtnDeleteTree);

	CEGUI::PushButton* pBtnQuit = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/QuitToMenu");
	pBtnQuit->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 55 ), CEGUI::UDim( 0, 30 ) ) );
	pBtnQuit->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 0, 170 ) ) );
	pBtnQuit->setText( "Quit" );
	pBtnQuit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbReturnToMenu, this));
	myRoot->addChildWindow(pBtnQuit);


	CEGUI::Window* pWndProperties = (CEGUI::Window*)wmgr.createWindow("TaharezLook/FrameWindow","Edit/Properties");
	pWndProperties->setSize(CEGUI::UVector2(CEGUI::UDim(0, 130), CEGUI::UDim(1, -180 - 110 -50)));
	pWndProperties->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 210)));
	pWndProperties->setText("Properties");
	pWndProperties->setProperty("SizingEnabled", "False");
	pWndProperties->setProperty("CloseButtonEnabled", "False");
	pWndProperties->setProperty("DragMovingEnabled", "False");
	CEGUI::ScrollablePane* pPropertiesPane = (CEGUI::ScrollablePane*)wmgr.createWindow("TaharezLook/ScrollablePane", "Edit/Properties/Pane");
	pPropertiesPane->setSize(CEGUI::UVector2( CEGUI::UDim( 1, -10),    CEGUI::UDim( 1, -40 ) ) );
	pPropertiesPane->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, 10),    CEGUI::UDim( 0, 40 ) ) );
	pWndProperties->addChildWindow(pPropertiesPane);
	myRoot->addChildWindow(pWndProperties);


	CEGUI::TabControl* pPalette = (CEGUI::TabControl*)wmgr.createWindow("TaharezLook/TabControl", "Edit/Palette");
	pPalette->setSize(CEGUI::UVector2( CEGUI::UDim( 1, -20 ), CEGUI::UDim( 0, 110 ) ) );
	pPalette->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10), CEGUI::UDim( 1.0f, -120 ) ) );
	
	CEGUI::DefaultWindow* pTabCores = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/TabContentPane");
		pTabCores->setProperty("EnableBottom","1");
		pTabCores->setText("Cores");
		{
			float width = 2;
			float height = 1;
			boost::filesystem::directory_iterator end_itr;	
			for(boost::filesystem::directory_iterator itr = boost::filesystem::directory_iterator("./Scripts/Sections");
				itr != end_itr;
				++itr)
			{
				if(boost::filesystem::is_regular((itr->status())))
				{
					std::string ext = boost::filesystem::extension(*itr);
					if(ext == ".XMLCore")
					{
						std::string filename = boost::filesystem::basename(itr->path());
						AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbSetCoreToXMLCore, this), filename.c_str(), pTabCores, width, height);
					}
				}
			}
		}
	pPalette->addTab(pTabCores);

	CEGUI::DefaultWindow* pTabWeapons = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/TabContentPane");
	pTabWeapons->setProperty("EnableBottom","1");
	pTabWeapons->setText("Weapons");
	{
		float width = 2;
		float height = 1;
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "Blaster", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "HeatBeamGun", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "HomingMissile", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "Swarmer", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "ChainGun", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "PlasmaArtillery", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "JointAngles", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "JointTracker", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "JointSpinner", pTabWeapons, width, height);
		AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), "TrackerArm", pTabWeapons, width, height);
	}
	pPalette->addTab(pTabWeapons);

	CEGUI::DefaultWindow* pTabXML = (CEGUI::DefaultWindow*)wmgr.createWindow("TaharezLook/TabContentPane");
	pTabXML->setProperty("EnableBottom","1");
	pTabXML->setText("XML Sections");
	CEGUI::ScrollablePane* pTabXMLPane = (CEGUI::ScrollablePane*)wmgr.createWindow("TaharezLook/ScrollablePane");
	pTabXMLPane->setSize(CEGUI::UVector2( CEGUI::UDim( 1, 0),    CEGUI::UDim( 1, 0 ) ) );
	pTabXML->addChildWindow(pTabXMLPane);
	{
		float width = 2;
		float height = 1;
		boost::filesystem::directory_iterator end_itr;	
		for(boost::filesystem::directory_iterator itr = boost::filesystem::directory_iterator("./Scripts/Sections");
			itr != end_itr;
			++itr)
		{
			if(boost::filesystem::is_regular((itr->status())))
			{
				std::string ext = boost::filesystem::extension(*itr);
				if(ext == ".XMLSection")
				{
					std::string filename = boost::filesystem::basename(itr->path());
					AddItemToTab(CEGUI::Event::Subscriber(&EditorScene::cbAddSection, this), filename.c_str(), static_cast<CEGUI::Window*>(pTabXMLPane), width, height);
				}
			}
		}
	}
	pPalette->addTab(pTabXML);
	myRoot->addChildWindow(pPalette);



	CEGUI::Window* pWndLoad = wmgr.createWindow( "TaharezLook/FrameWindow", "Edit/LoadDialogue" );
	pWndLoad->setVisible(false);
	pWndLoad->setModalState(false);
	pWndLoad->setProperty("SizingEnabled", "False");
	pWndLoad->setProperty("CloseButtonEnabled", "False");
	pWndLoad->setProperty("DragMovingEnabled", "False");
	pWndLoad->setText("Load your ship");
	pWndLoad->setSize( CEGUI::UVector2( CEGUI::UDim( 1, -60 ), CEGUI::UDim( 1, -60 ) ) );
	pWndLoad->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 30 ), CEGUI::UDim( 0, 30 ) ) );
	{
		CEGUI::Window* pBtnLoadDialogueLoad = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/LoadDialogue/Load");
		pBtnLoadDialogueLoad->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 60 ), CEGUI::UDim( 0, 30 ) ) );
		pBtnLoadDialogueLoad->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -70 ), CEGUI::UDim( 1, -40 ) ) );
		pBtnLoadDialogueLoad->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbLoadDialogueLoad, this));
		pBtnLoadDialogueLoad->setText("Load");
		pWndLoad->addChildWindow(pBtnLoadDialogueLoad);

		CEGUI::Window* pBtnLoadDialogueCancel = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/LoadDialogue/Cancel");
		pBtnLoadDialogueCancel->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 60 ), CEGUI::UDim( 0, 30 ) ) );
		pBtnLoadDialogueCancel->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -140 ), CEGUI::UDim( 1, -40 ) ) );
		pBtnLoadDialogueCancel->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbLoadDialogueCancel, this));
		pBtnLoadDialogueCancel->setText("Cancel");
		pWndLoad->addChildWindow(pBtnLoadDialogueCancel);

		CEGUI::Listbox* pLbShips = (CEGUI::Listbox*)wmgr.createWindow( "TaharezLook/Listbox", "Edit/LoadDialogue/Shiplist" );
		pLbShips->setPosition(CEGUI::UVector2(CEGUI::UDim(0,10),CEGUI::UDim(0,40)));
		pLbShips->setSize(CEGUI::UVector2(CEGUI::UDim(1,-20), CEGUI::UDim(1,-90)));
		pWndLoad->addChildWindow(pLbShips);
	}
	

	myRoot->addChildWindow(pWndLoad);


	CEGUI::Window* pWndSave = wmgr.createWindow( "TaharezLook/FrameWindow", "Edit/SaveDialogue" );
	pWndSave->setVisible(false);
	pWndSave->setModalState(false);
	pWndSave->setProperty("SizingEnabled", "False");
	pWndSave->setProperty("CloseButtonEnabled", "False");
	pWndSave->setProperty("DragMovingEnabled", "False");
	pWndSave->setText("Save your ship");
	pWndSave->setSize( CEGUI::UVector2( CEGUI::UDim( 1, -60 ), CEGUI::UDim( 1, -60 ) ) );
	pWndSave->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 30 ), CEGUI::UDim( 0, 30 ) ) );
	{
		CEGUI::Window* pBtnSaveDialogueSave = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/SaveDialogue/Save");
		pBtnSaveDialogueSave->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 60 ), CEGUI::UDim( 0, 30 ) ) );
		pBtnSaveDialogueSave->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -70 ), CEGUI::UDim( 1, -40 ) ) );
		pBtnSaveDialogueSave->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbSaveDialogueSave, this));
		pBtnSaveDialogueSave->setText("Save");
		pWndSave->addChildWindow(pBtnSaveDialogueSave);

		CEGUI::Window* pBtnSaveDialogueCancel = (CEGUI::PushButton*)wmgr.createWindow("TaharezLook/Button","Edit/SaveDialogue/Cancel");
		pBtnSaveDialogueCancel->setSize( CEGUI::UVector2( CEGUI::UDim( 0, 60 ), CEGUI::UDim( 0, 30 ) ) );
		pBtnSaveDialogueCancel->setPosition( CEGUI::UVector2( CEGUI::UDim( 1, -140 ), CEGUI::UDim( 1, -40 ) ) );
		pBtnSaveDialogueCancel->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&EditorScene::cbSaveDialogueCancel, this));
		pBtnSaveDialogueCancel->setText("Cancel");
		pWndSave->addChildWindow(pBtnSaveDialogueCancel);

		CEGUI::Window* pTbxFilename = (CEGUI::Editbox*)wmgr.createWindow("TaharezLook/Editbox","Edit/SaveDialogue/Filename");
		pTbxFilename->setSize( CEGUI::UVector2( CEGUI::UDim( 1, -160 ), CEGUI::UDim( 0, 30 ) ) );
		pTbxFilename->setPosition( CEGUI::UVector2( CEGUI::UDim( 0, 10 ), CEGUI::UDim( 1, -40 ) ) );
		pTbxFilename->setText("Filename");
		pWndSave->addChildWindow(pTbxFilename);

		CEGUI::Listbox* pLbShips = (CEGUI::Listbox*)wmgr.createWindow( "TaharezLook/Listbox", "Edit/SaveDialogue/Shiplist" );
		pLbShips->setPosition(CEGUI::UVector2(CEGUI::UDim(0,10),CEGUI::UDim(0,40)));
		pLbShips->setSize(CEGUI::UVector2(CEGUI::UDim(1,-20), CEGUI::UDim(1,-90)));
			pLbShips->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&EditorScene::cbSaveDialogueListSelected, this));
		pWndSave->addChildWindow(pLbShips);
	}
	

	myRoot->addChildWindow(pWndSave);

}

EditorScene::~EditorScene(void)
{
	CEGUI::WindowManager::getSingleton().destroyWindow("Edit/Root");
	CEGUI::WindowManager::getSingleton().destroyWindow("Edit/QuitToMenu");
	delete game_;
}

void EditorScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	if(sum_time_ == 0)
		CEGUI::System::getSingleton().setGUISheet( CEGUI::WindowManager::getSingleton().getWindow("Edit/Root"));
	sum_time_ += _timespan;
	fade_out_time_ -= _timespan;

	game_->Tick(_timespan);

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

void EditorScene::SetSelected(Section_ptr _selection)
{
	selection_ = _selection;
	game_->SetSelectedSection(selection_);
	
	CEGUI::Window* pWndPropertiesPane = (CEGUI::ScrollablePane*)CEGUI::WindowManager::getSingleton().getWindow("Edit/Properties/Pane");
	CEGUI::ScrollablePane* pWndPropertiesInnerPane = (CEGUI::ScrollablePane*)CEGUI::WindowManager::getSingleton().getWindow(pWndPropertiesPane->getChildAtIdx(0)->getName());
	std::vector<Property*> properties;
	selection_->GetProperties(properties);
	//Clear properties window 
	
	int child_count = static_cast<int>(pWndPropertiesInnerPane->getChildCount());
	for(int child_id = 0; child_id < (child_count)/2; child_id++)
	{
		CEGUI::Window* pWndChildName = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow(std::string("Edit/Properties/Pane/Name") + boost::lexical_cast<std::string, int>(child_id));
		CEGUI::Window* pWndChildValue = (CEGUI::Window*)CEGUI::WindowManager::getSingleton().getWindow(std::string("Edit/Properties/Pane/Value") + boost::lexical_cast<std::string, int>(child_id));
		pWndPropertiesInnerPane->removeChildWindow(pWndChildName);
		delete (Property*)(pWndChildValue->getUserData());
		pWndPropertiesInnerPane->removeChildWindow(pWndChildValue);
		CEGUI::WindowManager::getSingleton().destroyWindow(pWndChildName);
		CEGUI::WindowManager::getSingleton().destroyWindow(pWndChildValue);
	}


	float height = 40;
	for(int i = 0; i < static_cast<int>(properties.size()); i++)
	{
		CEGUI::Window* pLabel = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().createWindow( "TaharezLook/StaticText", "Edit/Properties/Pane/Name" + boost::lexical_cast<std::string, int>(i) );
		pLabel->setText(properties[i]->GetDescription());
		pLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 10), CEGUI::UDim(0, height)));
		pLabel->setSize(CEGUI::UVector2(CEGUI::UDim(1, -20), CEGUI::UDim(0, 30)));
		pWndPropertiesInnerPane->addChildWindow(pLabel);

		height+=35;

		if(properties[i]->Type() == PropertyType::Float)
		{
			CEGUI::Editbox* pEdit = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().createWindow( "TaharezLook/Editbox", "Edit/Properties/Pane/Value" + boost::lexical_cast<std::string, int>(i) );
			pEdit->setText(boost::lexical_cast<std::string, float>(properties[i]->GetFloat()));
			pEdit->setUserData(properties[i]);
			pEdit->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 10), CEGUI::UDim(0, height)));
			pEdit->setSize(CEGUI::UVector2(CEGUI::UDim(1, -20), CEGUI::UDim(0, 30)));
			pEdit->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&EditorScene::cbPropertyChanged, this));
			pWndPropertiesInnerPane->addChildWindow(pEdit);
		} else
		{
			CEGUI::Combobox* pEdit = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Combobox", "Edit/Properties/Pane/Value" + boost::lexical_cast<std::string, int>(i));
			pEdit->setText(properties[i]->GetEnumerationItem());
			pEdit->setUserData(properties[i]);
			pEdit->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 10), CEGUI::UDim(0, height)));
			pEdit->setSize(CEGUI::UVector2(CEGUI::UDim(1, -20), CEGUI::UDim(0.5, 30)));
			std::pair<int, std::string> item;
			BOOST_FOREACH(item, properties[i]->GetEnumeration())
			{
				pEdit->addItem(new CEGUI::ListboxTextItem(item.second, 0, (void*)item.first));
			}
			

			pEdit->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&EditorScene::cbEnumeratedPropertyChanged, this));
			pEdit->setReadOnly(true);
			pWndPropertiesInnerPane->addChildWindow(pEdit);
		}



		height+=35;
	}
}

bool EditorScene::cbPropertyChanged(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& we = 	static_cast<const CEGUI::WindowEventArgs&>(e);
	Property* prop = (Property*)we.window->getUserData();
	try
	{
		prop->SetFloat(boost::lexical_cast<float, std::string>(we.window->getText().c_str()));
	} catch(boost::bad_lexical_cast e)
	{
		Logger::Instance() << "Unable to parse " << we.window->getText().c_str() << " to a float\n";
	}
	return true;
}

bool EditorScene::cbEnumeratedPropertyChanged(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& we = 	static_cast<const CEGUI::WindowEventArgs&>(e);
	Property* prop = (Property*)we.window->getUserData();
	const CEGUI::Combobox* cb = static_cast<const CEGUI::Combobox*>(we.window);
	prop->SetEnumerationValue((int)cb->getSelectedItem()->getUserData());	
	return true;
}
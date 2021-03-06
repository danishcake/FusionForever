#include "stdafx.h"
#include <ctime>
#include <vector>
#include <list>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <sdl.h>
#include <../RendererModules/OpenGLGUIRenderer/openglrenderer.h>
#include <CEGUI.h>

#include "BaseScene.h"
#include "MenuScene.h"
#include "TitleScene.h"
#include "FadeInScene.h"
#include "GameScene.h"
#include "Camera.h"
#include "Settings.h"
#include "SectionTypes.h"
#include <SoundManager.h>
#include <PlayerAI.h>

clock_t ltv_time;
std::vector<BaseScene_ptr> scene_stack;


void Redraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Camera::Instance().SetupCamera();

	std::vector<BaseScene_ptr>::iterator last_root = scene_stack.begin();
	for(std::vector<BaseScene_ptr>::iterator it = scene_stack.begin(); it!=scene_stack.end(); it++)
	{
		if((*it)->IsRoot())
		{
			last_root = it;
		}
	}

	for(std::vector<BaseScene_ptr>::iterator it = last_root; it!=scene_stack.end(); it++)
	{
		glClear(GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		(*it)->Draw();
		if((*it)->IsRoot())
		{
			CEGUI::System::getSingleton().renderGUI();
		}
		glPopMatrix();
	}
	
	glFlush();
	SDL_GL_SwapBuffers();
}

void Tick()
{
	clock_t current_time = clock();
	float time_elapsed = (float)(current_time - ltv_time) / (float)CLOCKS_PER_SEC;
	if(time_elapsed > 1.0f/40.0f)
		time_elapsed = 1.0f/40.0f;
	ltv_time = current_time;

	std::vector<BaseScene_ptr>::iterator last_root = scene_stack.begin();
	for(std::vector<BaseScene_ptr>::iterator it = scene_stack.begin(); it!=scene_stack.end(); it++)
	{
		if((*it)->IsRoot())
		{
			last_root = it;
		}
	}

	std::vector<BaseScene_ptr> scene_spawn;
	for(std::vector<BaseScene_ptr>::iterator it = last_root; it!=scene_stack.end(); it++)
	{
		(*it)->Tick(time_elapsed, scene_spawn);
	}

	BOOST_FOREACH(BaseScene_ptr scene, scene_spawn)
	{
		scene_stack.push_back(scene);
	}

	CEGUI::System::getSingleton().injectTimePulse(time_elapsed);

	SoundManager::Instance().Tick();
}

bool IsSceneRemovable(BaseScene_ptr _scene)
{
	if(_scene->IsRemovable())
	{
		delete _scene;
		return true;
	}
	return false;
}

void Cull()
{
	scene_stack.erase(std::remove_if(scene_stack.begin(), scene_stack.end(), IsSceneRemovable), scene_stack.end());
}

void handle_mouse_down(Uint8 button)
{
switch ( button )
	{
	// handle real mouse buttons
	case SDL_BUTTON_LEFT:
		CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
		break;
	case SDL_BUTTON_MIDDLE:
		CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton);
		break;
	case SDL_BUTTON_RIGHT:
		CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
		break;
	
	// handle the mouse wheel
	case SDL_BUTTON_WHEELDOWN:
		CEGUI::System::getSingleton().injectMouseWheelChange( -1 );
		Camera::Instance().ZoomOut();
		break;
	case SDL_BUTTON_WHEELUP:
		CEGUI::System::getSingleton().injectMouseWheelChange( +1 );
		Camera::Instance().ZoomIn();
		break;
	}
}

void handle_mouse_up(Uint8 button)
{
switch ( button )
	{
	case SDL_BUTTON_LEFT:
		CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
		break;
	case SDL_BUTTON_MIDDLE:
		CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton);
		break;
	case SDL_BUTTON_RIGHT:
		CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
		break;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Vector2<int> resolution = Settings::Instance().GetResolution();
	srand((unsigned int)time(NULL));
	Camera::Instance().SetAspectRatio(resolution.x, resolution.y);

	bool isFinished = false;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
	PlayerAI::LoadBindings();

	SDL_WM_SetCaption("Fusion Forever", "Fusion Forever");
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_Surface* screen = SDL_SetVideoMode(Camera::Instance().GetWindowWidth(), Camera::Instance().GetWindowHeight(), 32, SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF | (Settings::Instance().GetFullscreen() ? SDL_FULLSCREEN : 0));
	if(!screen)
	{
		Logger::ErrorOut() << "SDL_SetVideoMode failed, unable to open screen device. Exitting\n";
		return -1;
	}
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	SectionTypes::RegisterSections();
	ltv_time = clock();

	CEGUI::OpenGLRenderer* renderer = new CEGUI::OpenGLRenderer(0, Camera::Instance().GetWindowWidth(), Camera::Instance().GetWindowHeight());
	new CEGUI::System(renderer);

	SDL_ShowCursor(SDL_DISABLE);
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// load in the scheme file, which auto-loads the TaharezLook imageset
	try
	{
		CEGUI::SchemeManager::getSingleton().loadScheme( "TaharezLook.scheme" );
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::ImagesetManager::getSingleton().createImagesetFromImageFile("Logo", "Textures/Logo.texture");

	} catch(CEGUI::Exception e)
	{
		Logger::ErrorOut() << e.getMessage().c_str() << "\n";
		Logger::ErrorOut() << "Unable to open TaharezLook.scheme, is it missing, or is the working directory wrong?\n";
		isFinished = true;
	}

	// load in a font.  The first font loaded automatically becomes the default font.
	try
	{
		if(!CEGUI::FontManager::getSingleton().isFontPresent( "Commonwealth-10" ) )
			CEGUI::FontManager::getSingleton().createFont( "Commonwealth-10.font" );
		CEGUI::System::getSingleton().setDefaultFont( "Commonwealth-10" );
		CEGUI::FontManager::getSingleton().notifyScreenResolution(CEGUI::Size(static_cast<float>(Camera::Instance().GetWindowWidth()), static_cast<float>(Camera::Instance().GetWindowHeight())));
	} catch(CEGUI::Exception e)
	{
		Logger::ErrorOut() << e.getMessage().c_str() << "\n";
		Logger::ErrorOut() << "Unable to open Commonwealth-10.font, is it missing, or is the working directory wrong?\n";
		isFinished = true;
	}

	CEGUI::System::getSingleton().setDefaultTooltip("TaharezLook/Tooltip");

	Logger::DiagnosticOut() << "Command line arguments\n";
	std::vector<std::string> args;
	for(int i = 0; i < argc; i++)
	{
		char conv[1000];
		wcstombs(conv, argv[i],1000);
		Logger::DiagnosticOut() << conv << "\n";
		args.push_back(std::string(conv));
	}

	if(!isFinished)
	{
		if(args.size() == 3 && !args[1].compare("-s"))
		{
			scene_stack.push_back(new GameScene(args[2]));
			scene_stack.push_back(BaseScene_ptr(new FadeInScene()));
		} else
		{
			scene_stack.push_back(BaseScene_ptr(new TitleScene()));
			scene_stack.push_back(BaseScene_ptr(new FadeInScene()));
		}

	}

	SDL_Event sdl_event;
	while(!isFinished)
	{
		if(scene_stack.empty())
			isFinished = true;
		while(SDL_PollEvent(& sdl_event))
		{
			// we use a switch to determine the event type
			switch (sdl_event.type)
			{
			// mouse motion handler
			case SDL_MOUSEMOTION:
				// we inject the mouse position directly.
				CEGUI::System::getSingleton().injectMousePosition(
					static_cast<float>(sdl_event.motion.x),
					static_cast<float>(sdl_event.motion.y)
				);
				break;
			
			// mouse down handler
			case SDL_MOUSEBUTTONDOWN:
				// let a special function handle the mouse button down event
				handle_mouse_down(sdl_event.button.button);
				break;
			
			// mouse up handler
			case SDL_MOUSEBUTTONUP:
				// let a special function handle the mouse button up event
				handle_mouse_up(sdl_event.button.button);
				break;
			
			
			// key down
			case SDL_KEYDOWN:
				// to tell CEGUI that a key was pressed, we inject the scancode.
				CEGUI::System::getSingleton().injectKeyDown(sdl_event.key.keysym.scancode);
				
				// as for the character it's a litte more complicated. we'll use for translated unicode value.
				// this is described in more detail below.
				if (sdl_event.key.keysym.unicode != 0)
				{
					CEGUI::System::getSingleton().injectChar(sdl_event.key.keysym.unicode);
				}
				break;
			
			// key up
			case SDL_KEYUP:
				// like before we inject the scancode directly.
				CEGUI::System::getSingleton().injectKeyUp(sdl_event.key.keysym.scancode);
				break;
			
			
			// WM quit event occured
			case SDL_QUIT:
				isFinished = true;
				break;
			
			}
		}
		Tick();
		Redraw();
		Cull();
	}
	PlayerAI::SaveBindings();
	SDL_Quit();

	return 0;
}


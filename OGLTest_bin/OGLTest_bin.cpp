#include "stdafx.h"
#include <ctime>
#include <sdl.h>
#include <AntTweakBar.h>

#include <vector>
#include <list>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "BaseScene.h"
#include "MenuScene.h"
#include "FadeInScene.h"
#include "Camera.h"

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
			TwDraw();
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

int _tmain(int argc, _TCHAR* argv[])
{
	srand((unsigned int)time(NULL));
	Camera::Instance().SetAspectRatio(500,500);

	bool isFinished = false;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SDL Test", "SDL Test");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface* screen = SDL_SetVideoMode(Camera::Instance().GetWindowWidth(), Camera::Instance().GetWindowHeight(), 32, SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF);

	TwInit(TW_OPENGL, NULL);
	TwWindowSize(Camera::Instance().GetWindowWidth(), Camera::Instance().GetWindowHeight());
	//TwDeleteAllBars(); //I would very much like to do this, but there is a bug whereby resizing menus after this causes a crash :(

	scene_stack.push_back(BaseScene_ptr(new MenuScene()));
	scene_stack.push_back(BaseScene_ptr(new FadeInScene()));


	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ltv_time = clock();

	SDL_Event sdl_event;
	while(!isFinished)
	{
		if(SDL_PollEvent(& sdl_event))
		{
			int handled = TwEventSDL(&sdl_event); // send event to AntTweakBar
			if( !handled )                // if event has not been handled by AntTweakBar, process it
			{
				switch(sdl_event.type)
				{
					case SDL_QUIT:
						isFinished = true;
					break;
				}
			}
		}
		Sleep(5);

		Tick();
		Redraw();
		SDL_Flip(screen);
		Cull();
	}
	TwTerminate();
	SDL_Quit();

	return 0;
}

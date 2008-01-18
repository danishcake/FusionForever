// OGLTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "sdl.h"

#include <vector>
#include <list>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "BaseScene.h"
#include "GameScene.h"
#include "FadeInScene.h"
#include "Camera.h"

#define WINDOW_SIZE 200

clock_t ltv_time;
std::vector<boost::shared_ptr<BaseScene>> scene_stack;

void Redraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Camera::Instance().SetupCamera();

	std::vector<boost::shared_ptr<BaseScene>>::iterator last_root = scene_stack.begin();
	for(std::vector<boost::shared_ptr<BaseScene>>::iterator it = scene_stack.begin(); it!=scene_stack.end(); it++)
	{
		if((*it)->IsRoot())
		{
			last_root = it;
		}
	}

	for(std::vector<boost::shared_ptr<BaseScene>>::iterator it = last_root; it!=scene_stack.end(); it++)
	{
		glClear(GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		(*it)->Draw();
		glPopMatrix();
	}
	glFlush();
	SDL_GL_SwapBuffers();
}

void Tick()
{
	clock_t current_time = clock();
	float time_elapsed = (float)(current_time - ltv_time) / (float)CLOCKS_PER_SEC;
	ltv_time = current_time;

	std::vector<boost::shared_ptr<BaseScene>>::iterator last_root = scene_stack.begin();
	for(std::vector<boost::shared_ptr<BaseScene>>::iterator it = scene_stack.begin(); it!=scene_stack.end(); it++)
	{
		if((*it)->IsRoot())
		{
			last_root = it;
		}
	}

	for(std::vector<boost::shared_ptr<BaseScene>>::iterator it = last_root; it!=scene_stack.end(); it++)
	{
		(*it)->Tick(time_elapsed, scene_stack);
	}
}

bool IsSceneRemovable(boost::shared_ptr<BaseScene> _scene)
{
	return _scene->IsRemovable();
}

void Cull()
{
	scene_stack.erase(std::remove_if(scene_stack.begin(), scene_stack.end(), IsSceneRemovable), scene_stack.end());
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand((unsigned int)time(NULL));
	Camera::Instance().SetAspectRatio(300,300);
	Camera::Instance().SetWidth(200);

	bool isFinished = false;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SDL Test", "SDL Test");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface* screen = SDL_SetVideoMode(Camera::Instance().GetWindowWidth(), Camera::Instance().GetWindowHeight(), 32, SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF);
	scene_stack.push_back(boost::shared_ptr<BaseScene>(new GameScene()));
	scene_stack.push_back(boost::shared_ptr<BaseScene>(new FadeInScene()));
	

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ltv_time = clock();

	SDL_Event sdl_event;
	while(!isFinished)
	{
		if(SDL_PollEvent(& sdl_event))
		{
			switch(sdl_event.type)
			{
				case SDL_QUIT:
					isFinished = true;
				break;
			}
		}
		Sleep(5);

		Tick();
		Redraw();
		SDL_Flip(screen);
		Cull();
	}
	SDL_Quit();

	return 0;
}


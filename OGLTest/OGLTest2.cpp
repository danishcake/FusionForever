// OGLTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "sdl.h"

#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "BaseScene.h"
#include "GameScene.h"
#include "Camera.h"

#define WINDOW_SIZE 200

clock_t ltv_time;
BaseScene * scene;
std::vector<boost::shared_ptr<BaseScene>> scene_stack;

void Redraw()
{
	scene->Draw();
}

void Tick()
{
	clock_t current_time = clock();
	float time_elapsed = (float)(current_time - ltv_time) / (float)CLOCKS_PER_SEC;
	ltv_time = current_time;
	scene->Tick(time_elapsed, scene_stack);
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));
		Camera::Instance().SetAspectRatio(300,300);
	Camera::Instance().SetWidth(200);

	bool isFinished = false;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SDL Test", "SDL Test");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface* screen = SDL_SetVideoMode(Camera::Instance().GetWindowWidth(), Camera::Instance().GetWindowHeight(), 32, SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF);
	scene = new GameScene();
	
	glClearColor(0.0f,0.0f,0.7f,0.0f);
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
	}
	SDL_Quit();

	return 0;
}


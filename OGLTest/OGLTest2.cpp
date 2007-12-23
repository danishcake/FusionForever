// OGLTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "sdl.h"

#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "Core.h"

#include "SquareCore.h"
#include "RotatingAI.h"
#include "KeyboardAI.h"
#include "RigidArm.h"
#include "Blaster.h"

#define WINDOW_SIZE 200

clock_t ltv_time;

typedef boost::shared_ptr<Projectile> Projectile_ptr;
typedef boost::shared_ptr<Decoration> Decoration_ptr;

std::list<Projectile_ptr> enemy_projectiles = std::list<Projectile_ptr>();
std::list<Projectile_ptr> ownship_projectiles = std::list<Projectile_ptr>();
std::list<Decoration_ptr> decorations = std::list<Decoration_ptr>();

std::list<boost::shared_ptr<Core>> friends;
std::list<boost::shared_ptr<Core>> enemies;

void Redraw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f,0.0f,0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-60,60,-60, 60,-100,100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	BOOST_FOREACH(boost::shared_ptr<Core> core, friends)
	{
		core->DrawSelf();
	}

	BOOST_FOREACH(boost::shared_ptr<Core> core, enemies)
	{
		core->DrawSelf();
	}

	for(std::list<Projectile_ptr>::iterator it = enemy_projectiles.begin(); it != enemy_projectiles.end(); it++)
	{
		(*it)->DrawSelf();
	}

	for(std::list<Projectile_ptr>::iterator it = ownship_projectiles.begin(); it != ownship_projectiles.end(); it++)
	{
		(*it)->DrawSelf();
	}

	for(std::list<Decoration_ptr>::iterator it = decorations.begin(); it != decorations.end(); it++)
	{
		(*it)->DrawSelf();
	}

	glFlush();
}

void Tick()
{
	std::list<boost::shared_ptr<Projectile>> enemy_spawn;
	std::list<boost::shared_ptr<Projectile>> ownship_spawn;
	std::list<boost::shared_ptr<Decoration>> decoration_spawn;
	clock_t current_time = clock();
	float time_elapsed = (float)(current_time - ltv_time) / (float)CLOCKS_PER_SEC;
	ltv_time = current_time;
	const Matrix4f identity = Matrix4f();


	BOOST_FOREACH(boost::shared_ptr<Core> core, friends)
	{
		core->Tick(time_elapsed, ownship_spawn, decoration_spawn, identity, friends , enemies);
	}

	BOOST_FOREACH(boost::shared_ptr<Core> core, enemies)
	{
		core->Tick(time_elapsed, enemy_spawn, decoration_spawn, identity, enemies, friends);
	}

	enemy_projectiles.splice(enemy_projectiles.begin(), enemy_spawn);
	ownship_projectiles.splice(ownship_projectiles.begin(), ownship_spawn);

	for(std::list<Projectile_ptr>::iterator it = enemy_projectiles.begin(); it != enemy_projectiles.end(); it++)
	{
		(*it)->Tick(time_elapsed, identity);
		
		BOOST_FOREACH(boost::shared_ptr<Core> core, friends)
		{
			core->CheckCollisions(*it);
			if((*it)->GetLifetime()<=0)
			{
				(*it)->Hit(decoration_spawn);
				break;
			}
		}
	}

	for(std::list<Projectile_ptr>::iterator it = ownship_projectiles.begin(); it != ownship_projectiles.end(); it++)
	{
		(*it)->Tick(time_elapsed, identity);
		
		BOOST_FOREACH(boost::shared_ptr<Core> core, enemies)
		{
			core->CheckCollisions(*it);
			if((*it)->GetLifetime()<=0)
			{
				(*it)->Hit(decoration_spawn);
				break;
			}
		}
		
	}

	for(std::list<Decoration_ptr>::iterator it = decorations.begin(); it != decorations.end(); it++)
	{
		(*it)->Tick(time_elapsed, identity);
	}


	decorations.splice(decorations.begin(), decoration_spawn);

	enemy_projectiles.remove_if(Projectile::IsProjectileRemovable);
	ownship_projectiles.remove_if(Projectile::IsProjectileRemovable);
	decorations.remove_if(Decoration::IsRemovable);
}

void initSections()
{
	boost::shared_ptr<Core> core = boost::shared_ptr<Core>(new SquareCore(new KeyboardAI()));
	friends.push_back(core);

	Section* s_s;
	Blaster * b;

	s_s = new RigidArm();
	s_s->SetPosition(Vector3f(-4,10,0));
	b = new Blaster();
	b->SetPosition(Vector3f(0,7.5,0));
	s_s->AddChild(b);
	core->AddChild(s_s);
	
	s_s = new RigidArm();
	s_s->SetPosition(Vector3f(4,10,0));
	b = new Blaster();
	b->SetPosition(Vector3f(0,7.5,0));
	s_s->AddChild(b);

	b = new Blaster();
	b->SetPosition(Vector3f(10,2,0));
	b->SetAngle(-30);
	core->AddChild(b);

	b = new Blaster();
	b->SetPosition(Vector3f(-10,2,0));
	b->SetAngle(30);
	core->AddChild(b);
	
	core->AddChild(s_s);
	core->SetColor(GLColor(0,128,0));


	boost::shared_ptr<Core> en_core = boost::shared_ptr<Core>(new SquareCore(new RotatingAI(1)));
	s_s = new RigidArm();
	s_s->SetPosition(Vector3f(4,10,0));
	en_core->AddChild(s_s);
	s_s = new RigidArm();
	s_s->SetPosition(Vector3f(-4,10,0));
	en_core->AddChild(s_s);
	s_s = new RigidArm();
	s_s->SetPosition(Vector3f(4,-10,0));
	s_s->SetAngle(180);
	en_core->AddChild(s_s);
	s_s = new RigidArm();
	s_s->SetPosition(Vector3f(-4,-10,0));
	s_s->SetAngle(180);
	en_core->AddChild(s_s);

	en_core->SetPosition(Vector3f(20,20,0));


	enemies.push_back(en_core);



	
}

int _tmain(int argc, _TCHAR* argv[])
{
	bool isFinished = false;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SDL Test", "SDL Test");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
	SDL_Surface* screen = SDL_SetVideoMode(200, 200, 32, SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF);

	initSections();
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


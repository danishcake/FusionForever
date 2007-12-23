// OGLTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "sdl.h"
#include "Section.h"
#include "Core.h"
#include "JointSpin.h"
#include "RigidArm.h"
#include "JointAngles.h"
#include "GattlingGun.h"

#include "Projectile.h"

#include "Decoration.h"
//#include "Puff.h"

#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#define WINDOW_SIZE 200

Core enemy;
Core ownship;
clock_t ltv_time;

typedef boost::shared_ptr<Projectile> Projectile_ptr;
typedef boost::shared_ptr<Decoration> Decoration_ptr;

std::list<Projectile_ptr> enemy_projectiles = std::list<Projectile_ptr>();
std::list<Projectile_ptr> ownship_projectiles = std::list<Projectile_ptr>();
std::list<Decoration_ptr> decorations = std::list<Decoration_ptr>();

void Test();

void CALLBACK Redraw()
{
	
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f,0.0f,0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-70,70,-70,70,-100,100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	enemy.DrawSelf();
	ownship.DrawSelf();

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

bool IsProjectileRemovable(boost::shared_ptr<Projectile> prj)
{
	return (prj->LifeTime <= 0);
}

bool IsDecorationRemovable(boost::shared_ptr<Decoration> dec)
{
	return (dec->LifeTime <= 0);
}

void CALLBACK Tick()
{
	clock_t current_time = clock();
	float time_elapsed = (float)(current_time - ltv_time) / (float)CLOCKS_PER_SEC;
	ltv_time = current_time;
	std::list<boost::shared_ptr<Projectile>> enemy_spawn;// = std::list<boost::shared_ptr<Projectile>>();
	std::list<boost::shared_ptr<Projectile>> ownship_spawn;
	std::list<boost::shared_ptr<Decoration>> decoration_spawn;
	enemy.Tick(time_elapsed, enemy_spawn, decoration_spawn, Matrix4f());
	enemy.SetTarget(ownship.Position);

	ownship.Tick(time_elapsed, ownship_spawn, decoration_spawn, Matrix4f()); 
	ownship.SetTarget(enemy.Position);


	enemy_projectiles.splice(enemy_projectiles.begin(), enemy_spawn);
	ownship_projectiles.splice(ownship_projectiles.begin(), ownship_spawn);

	//TODO move loops to preincrement?
	for(std::list<Projectile_ptr>::iterator it = enemy_projectiles.begin(); it != enemy_projectiles.end(); it++)
	{
		(*it)->Tick(time_elapsed, decoration_spawn);
		ownship.CheckCollisions(*it);
		if((*it)->LifeTime<=0)
			(*it)->Hit(decoration_spawn);
	}

	for(std::list<Projectile_ptr>::iterator it = ownship_projectiles.begin(); it != ownship_projectiles.end(); it++)
	{
		(*it)->Tick(time_elapsed, decoration_spawn);
		enemy.CheckCollisions(*it);
		if((*it)->LifeTime<=0)
			(*it)->Hit(decoration_spawn);
	}

	for(std::list<Decoration_ptr>::iterator it = decorations.begin(); it != decorations.end(); it++)
	{
		(*it)->Tick(time_elapsed);
	}

	decorations.splice(decorations.begin(), decoration_spawn);

	enemy_projectiles.remove_if(IsProjectileRemovable);
	ownship_projectiles.remove_if(IsProjectileRemovable);
	decorations.remove_if(IsDecorationRemovable);
}




void initSections()
{

	enemy.Position = Vector3f(-50,0,0);
	enemy.SetColor(GLColor(255,0,0));

	Section* sub_section = new RigidArm(); 
	sub_section->Position = Vector3f(10,-5,0);
	sub_section->Angle= 270;
	enemy.AddChild(sub_section);
	
	Section* rot_arm = new JointAngles(-90.0f, 90.0f, 2.3f, 1.8f);
	rot_arm->Position = Vector3f(0.0f, 7.5f, 0.0f);
	sub_section->AddChild(rot_arm);

	sub_section = new RigidArm();
	sub_section->Position = Vector3f(0,2.5f,0);
	rot_arm->AddChild(sub_section);

	sub_section = new RigidArm();
	sub_section->Position = Vector3f(-10,-5,0);
	sub_section->Angle = 90;
	enemy.AddChild(sub_section);

	Section* sub_sub_section = new RigidArm();
	sub_sub_section->Position = Vector3f(0,7.5f,0);
	//sub_sub_section->angle_ = 0;
	sub_section->AddChild(sub_sub_section);

	Section* joint = new JointSpin(FastReverse);
	joint->Position = Vector3f(0,7.5f,0);
	sub_sub_section->AddChild(joint);

	sub_section = new RigidArm();
	sub_section->Position = Vector3f(0,2.5f,0);
	sub_section->Angle = 0.0f;
	joint->AddChild(sub_section);

	sub_section = new RigidArm();
	sub_section->Position = Vector3f(-2.5f,0 ,0);
	sub_section->Angle = 90.0f;
	joint->AddChild(sub_section);

	sub_section = new RigidArm();
	sub_section->Position = Vector3f(0,-2.5f,0);
	sub_section->Angle = 180.0f;
	joint->AddChild(sub_section);

	sub_section = new RigidArm();
	sub_section->Position = Vector3f(2.5f,0,0);
	sub_section->Angle = 270.0f;
	joint->AddChild(sub_section);





	//Setup ownship
	ownship.Position = Vector3f(50,0,0);
	ownship.SetColor(GLColor(0,0,255));
	//Top
	GattlingGun* gun = new GattlingGun();
	gun->Position = Vector3f(0,10,0);
	gun->Angle = 0;
	ownship.AddChild(gun);

	gun = new GattlingGun();
	gun->Position = Vector3f(-2.5f,10,0);
	gun->Angle = 10;
	ownship.AddChild(gun);

	gun = new GattlingGun();
	gun->Position = Vector3f(2.5f,10,0);
	gun->Angle = -10;
	ownship.AddChild(gun);

	//Right
	gun = new GattlingGun();
	gun->Position = Vector3f(10,0,0);
	gun->Angle = 270.0f;
	ownship.AddChild(gun);
	
	gun = new GattlingGun();
	gun->Position = Vector3f(10, -2.5f ,0);
	gun->Angle = 260.0f;
	ownship.AddChild(gun);
	
	gun = new GattlingGun();
	gun->Position = Vector3f(10,2.5f,0);
	gun->Angle = 280.0f;
	ownship.AddChild(gun);

	//Left
	gun = new GattlingGun();
	gun->Position = Vector3f(-10,0,0);
	gun->Angle = 90.0f;
	ownship.AddChild(gun);
	
	gun = new GattlingGun();
	gun->Position = Vector3f(-10, -2.5f ,0);
	gun->Angle = 100.0f;
	ownship.AddChild(gun);
	
	gun = new GattlingGun();
	gun->Position = Vector3f(-10,2.5f,0);
	gun->Angle = 80.0f;
	ownship.AddChild(gun);


}

int _tmain(int argc, _TCHAR* argv[])
{	
	bool isFinished = false;
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("SDL Test", "SDL Test");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,0);
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
		Sleep(10);
		Tick();
		Redraw();
		SDL_Flip(screen);
	}
	SDL_Quit();

	return 0;
}


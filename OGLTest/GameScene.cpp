#include "StdAfx.h"
#include "GameScene.h"

#include "Camera.h"
#include <boost/foreach.hpp>

#include "SquareCore.h"
#include "RotatingAI.h"
#include "KeyboardAI.h"
#include "RigidArm.h"
#include "Blaster.h"
#include "HeatBeamGun.h"
#include "SpinningJoint.h"
#include <lauxlib.h>

void GameScene::initSections()
{
}



GameScene::GameScene(void)
{
	initSections();
	Camera::Instance().SetWidth(500);
	game_lua_.LoadChallenge("Challenge1.lua");
}

GameScene::~GameScene(void)
{
}

void GameScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	game_lua_.Tick(friends_.size(), enemies_.size(), _timespan);

	enemies_.splice(enemies_.begin(), game_lua_.GetEnemies());
	friends_.splice(friends_.begin(), game_lua_.GetFriends());
	game_lua_.GetFriends().clear();
	game_lua_.GetEnemies().clear();

	std::list<Projectile_ptr> enemy_spawn;
	std::list<Projectile_ptr> ownship_spawn;
	std::list<Decoration_ptr> decoration_spawn;

	const Matrix4f identity = Matrix4f();

	Camera::Instance().TickCamera(_timespan);

	BOOST_FOREACH(Core_ptr core, friends_)
	{
		core->Tick(_timespan, ownship_spawn, decoration_spawn, identity, friends_ , enemies_);
	}

	BOOST_FOREACH(Core_ptr core, enemies_)
	{
		core->Tick(_timespan, enemy_spawn, decoration_spawn, identity, friends_ , enemies_);
	}

	enemy_projectiles.splice(enemy_projectiles.begin(), enemy_spawn);
	ownship_projectiles.splice(ownship_projectiles.begin(), ownship_spawn);

	for(std::list<Projectile_ptr>::iterator it = enemy_projectiles.begin(); it != enemy_projectiles.end(); it++)
	{
		(*it)->Tick(_timespan, identity);

		BOOST_FOREACH(Core_ptr core, friends_)
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
		(*it)->Tick(_timespan, identity);

		BOOST_FOREACH(Core_ptr core, enemies_)
		{
			core->CheckCollisions(*it);
			if((*it)->GetLifetime()<=0)
			{
				(*it)->Hit(decoration_spawn);
				break;
			}
		}
	}

	BOOST_FOREACH(Core_ptr core, friends_)
	{
		if(core->GetHealth() <= 0)
			core->GetDeathSpawn(decoration_spawn);
	}

	BOOST_FOREACH(Core_ptr core, enemies_)
	{
		if(core->GetHealth() <= 0)
			core->GetDeathSpawn(decoration_spawn);
	}

	decorations.splice(decorations.begin(), decoration_spawn);

	for(std::list<Decoration_ptr>::iterator it = decorations.begin(); it != decorations.end(); it++)
	{
		(*it)->Tick(_timespan, identity);
	}

	enemy_projectiles.remove_if(Projectile::IsProjectileRemovable);
	ownship_projectiles.remove_if(Projectile::IsProjectileRemovable);
	decorations.remove_if(Decoration::IsRemovable);
	friends_.remove_if(Section::IsRemovable);
	enemies_.remove_if(Section::IsRemovable);
}

void GameScene::Draw()
{
	glColor3f(0.0f,0.0f,0.0f);
	starfield_.DrawStarfield(Vector3f(Camera::Instance().GetFocusX(), Camera::Instance().GetFocusY(), 0));
	BOOST_FOREACH(Core_ptr core, friends_)
	{
		core->DrawSelf();
	}

	BOOST_FOREACH(Core_ptr core, enemies_)
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
}

bool GameScene::IsRoot()
{
	return true;
}

bool GameScene::IsRemovable()
{
	return false;
}
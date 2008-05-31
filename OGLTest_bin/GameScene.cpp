#include "StdAfx.h"
#include "GameScene.h"

#include "Camera.h"
#include <boost/foreach.hpp>

#include "SquareCore.h"
#include "RotatingAI.h"
#include "KeyboardAI.h"
#include <lauxlib.h>

GameScene::GameScene(void)
{
	Camera::Instance().SetWidth(500);
	game_lua_ = new GameLua(this);
	game_lua_->LoadChallenge("Challenge1.lua");
}

GameScene::~GameScene(void)
{
	delete game_lua_;
}

void GameScene::Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes)
{
	game_lua_->Tick(static_cast<int>(friends_.size()), static_cast<int>(enemies_.size()), _timespan);

	//enemies_.splice(enemies_.begin(), game_lua_->GetEnemies());
	enemies_.insert(enemies_.end(), game_lua_->GetEnemies().begin(), game_lua_->GetEnemies().end());
	//friends_.splice(friends_.begin(), game_lua_->GetFriends());
	friends_.insert(friends_.end(), game_lua_->GetFriends().begin(), game_lua_->GetFriends().end());
	
	game_lua_->GetFriends().clear();
	game_lua_->GetEnemies().clear();

	std::vector<Projectile_ptr> enemy_spawn;
	std::vector<Projectile_ptr> ownship_spawn;
	std::vector<Decoration_ptr> decoration_spawn;

	const Matrix4f identity = Matrix4f();

	Camera::Instance().TickCamera(_timespan);

	BOOST_FOREACH(Core_ptr core, friends_)
	{
		core->Tick(_timespan, ownship_spawn, decoration_spawn, identity, friends_ , enemies_);
	}

	BOOST_FOREACH(Core_ptr core, enemies_)
	{
		core->Tick(_timespan, enemy_spawn, decoration_spawn, identity, enemies_, friends_);
	}

	//enemy_projectiles.splice(enemy_projectiles.begin(), enemy_spawn);
	enemy_projectiles.insert(enemy_projectiles.begin(), enemy_spawn.begin(), enemy_spawn.end());
	//ownship_projectiles.splice(ownship_projectiles.begin(), ownship_spawn);
	ownship_projectiles.insert(ownship_projectiles.begin(), ownship_spawn.begin(), ownship_spawn.end());

	for(std::vector<Projectile_ptr>::iterator it = enemy_projectiles.begin(); it != enemy_projectiles.end(); it++)
	{
		(*it)->Tick(_timespan, decoration_spawn, identity);

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

	for(std::vector<Projectile_ptr>::iterator it = ownship_projectiles.begin(); it != ownship_projectiles.end(); it++)
	{
		(*it)->Tick(_timespan, decoration_spawn, identity);

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

	//decorations.splice(decorations.begin(), decoration_spawn);
	decorations.insert(decorations.end(), decoration_spawn.begin(), decoration_spawn.end());

	for(std::vector<Decoration_ptr>::iterator it = decorations.begin(); it != decorations.end(); it++)
	{
		(*it)->Tick(_timespan, identity);
	}
	enemy_projectiles.erase(std::remove_if(enemy_projectiles.begin(), enemy_projectiles.end(), Projectile::IsProjectileRemovable), enemy_projectiles.end());
	//enemy_projectiles.remove_if(Projectile::IsProjectileRemovable);
	
	ownship_projectiles.erase(std::remove_if(ownship_projectiles.begin(), ownship_projectiles.end(), Projectile::IsProjectileRemovable),ownship_projectiles.end());
	//ownship_projectiles.remove_if(Projectile::IsProjectileRemovable);
	
	//decorations.remove_if(Decoration::IsRemovable);
	decorations.erase(std::remove_if(decorations.begin(), decorations.end(), Decoration::IsRemovable), decorations.end());

	friends_.erase(std::remove_if(friends_.begin(), friends_.end(), Section::IsRemovable), friends_.end());
	//friends_.remove_if(Section::IsRemovable);
	
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), Section::IsRemovable), enemies_.end());
	//enemies_.remove_if(Section::IsRemovable);
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

	for(std::vector<Projectile_ptr>::iterator it = enemy_projectiles.begin(); it != enemy_projectiles.end(); it++)
	{
		(*it)->DrawSelf();
	}

	for(std::vector<Projectile_ptr>::iterator it = ownship_projectiles.begin(); it != ownship_projectiles.end(); it++)
	{
		(*it)->DrawSelf();
	}

	for(std::vector<Decoration_ptr>::iterator it = decorations.begin(); it != decorations.end(); it++)
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

bool GameScene::IsSectionAlive(int _section_id)
{
	BOOST_FOREACH(Core_ptr core, friends_)
	{
		if(core->GetSectionID() == _section_id)
		{
			return true;
		}
	}

	BOOST_FOREACH(Core_ptr core, enemies_)
	{
		if(core->GetSectionID() == _section_id)
		{
			return true;
		}
	}

	return false;
}
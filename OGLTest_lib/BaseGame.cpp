#include "StdAfx.h"
#include "BaseGame.h"
#include "LuaChallenge.h"
#include "LuaAI.h"
#include "Decoration.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


BaseGame::BaseGame(std::string _challenge_filename)
{
  for(int x = 0; x < MAX_FORCES; x++)
  {
    for(int y = 0; y < MAX_FORCES; y++)
    {
      hostility_[x][y] = (x != y) ? Hostility::Hostile : Hostility::Friendly;
    }
  }
  luaVM_ = lua_open();
  LuaAI::initialised_lua = false;
  luaL_openlibs(luaVM_);
  challenge_ = new LuaChallenge(luaVM_, _challenge_filename, this);

  
}

BaseGame::~BaseGame(void)
{
	delete challenge_;

	for(int force = 0; force < MAX_FORCES; force++)
	{
		BOOST_FOREACH(Projectile_ptr ptr, projectiles_[force])
		{
			delete ptr;
		}
		BOOST_FOREACH(Core_ptr ptr, ships_[force])
		{
			delete ptr;
		}
	}
	BOOST_FOREACH(Decoration_ptr ptr, decorations_)
	{
		delete ptr;
	}

	lua_close(luaVM_);
}

void BaseGame::Draw()
{
	starfield_.DrawStarfield(Vector3f(Camera::Instance().GetFocusX(), Camera::Instance().GetFocusY(), 0));
	for(int force = 0; force < MAX_FORCES; force++)
	{
		//collision_managers_[force].Render();
	}

	for(int force = 0; force < MAX_FORCES; force++)
	{
		BOOST_FOREACH(Core_ptr core, ships_[force])
		{
			core->DrawSelf();
		}
	}

	for(int force = 0; force < MAX_FORCES; force++)
	{
		BOOST_FOREACH(Projectile_ptr projectile, projectiles_[force])
		{
			projectile->DrawSelf();
		}
	}

	BOOST_FOREACH(Decoration_ptr decoration, decorations_)
	{
		decoration->DrawSelf();
	}
}

int BaseGame::Tick(float _timespan)
{
	std::vector<Decoration_ptr> decoration_spawn;
	std::vector<Section_ptr> filtered;
	filtered.reserve(50);
	decoration_spawn.reserve(20);

	//Tick scripts
	ChallengeState::Enum state = challenge_->Tick(_timespan);


	Camera::Instance().TickCamera(_timespan);

	//Calculate enemies and friends of each force
	for(int force = 0; force < MAX_FORCES; force++)
	{
		enemies[force].clear();
		friends[force].clear();
		for(int other_force = 0; other_force < MAX_FORCES; other_force++)
		{
			if(other_force != force)
			{
				if(hostility_[force][other_force] == Hostility::Hostile)
					enemies[force].insert(enemies[force].end(),ships_[other_force].begin(), ships_[other_force].end());
				if(hostility_[force][other_force] == Hostility::Friendly)
					friends[force].insert(friends[force].end(),ships_[other_force].begin(), ships_[other_force].end());
			}
		}
	}

	for(int force = 0; force < MAX_FORCES; force++)
	{
		collision_managers_[force].Clear();
	}

	//Tick ships
	const Matrix4f identity = Matrix4f();
	std::vector<Projectile_ptr> projectile_spawn;
	projectile_spawn.reserve(50);
	for(int force = 0; force < MAX_FORCES; force++)
	{
		projectile_spawn.clear();
		BOOST_FOREACH(Core_ptr core, ships_[force])
		{
			core->Tick(_timespan, projectile_spawn, decoration_spawn, identity, friends[force], enemies[force], &collision_managers_[force]);
		}
		//Add spawned projectiles
		projectiles_[force].insert(projectiles_[force].end(), projectile_spawn.begin(), projectile_spawn.end());
	}

	//Tick and collide projectiles
	for(int force = 0; force < MAX_FORCES; force++)
	{
		BOOST_FOREACH(Projectile_ptr projectile, projectiles_[force])
		{
			projectile->Tick(_timespan, decoration_spawn, identity);
			//Collect up all local enemies to test collisions against
			for(int other_force = 0; other_force < MAX_FORCES; other_force++)
			{
				filtered.clear();
				if(other_force != force && hostility_[force][other_force] == Hostility::Hostile)
				{
					collision_managers_[other_force].GetAtPoint(filtered, projectile->GetPosition());
					std::sort(filtered.begin(),filtered.end(), RelativeRangeSort<Projectile_ptr, Section_ptr>(projectile));
					BOOST_FOREACH(Section_ptr section, filtered)
					{
						if(section->CheckCollisions(projectile)) 
							break; //Checks the collisions and does damage
					}
				}
			}
			if(projectile->GetLifetime()<=0)
			{
				projectile->Hit(decoration_spawn);
			}
		}
	}
	//Get dead sections deathspawn
	for(int force = 0; force < MAX_FORCES; force++)
	{
		BOOST_FOREACH(Core_ptr core, ships_[force])
		{
			if(core->GetHealth() <= 0)
				core->GetDeathSpawn(decoration_spawn);
		}
	}
	//Add spawned decorations 
	decorations_.insert(decorations_.end(),decoration_spawn.begin(), decoration_spawn.end());
	//Tick decorations
	BOOST_FOREACH(Decoration_ptr decoration, decorations_)
	{
		decoration->Tick(_timespan, identity);
	}

	//Remove the dead
	decorations_.erase(std::remove_if(decorations_.begin(), decorations_.end(), Decoration::IsRemovable), decorations_.end());
	for(int force = 0; force < MAX_FORCES; force++)
	{
		projectiles_[force].erase(std::remove_if(projectiles_[force].begin(), projectiles_[force].end(), Projectile::IsProjectileRemovable), projectiles_[force].end());
		ships_[force].erase(std::remove_if(ships_[force].begin(), ships_[force].end(), Section::IsRemovable), ships_[force].end());
	}

	return state;
}

void BaseGame::AddShip(Core* _core, int _force)
{
	ships_[_force].push_back(_core);
}

int BaseGame::GetForceCount(int _force)
{
	return static_cast<int>(ships_[_force].size());
}

int BaseGame::GetEnemyCount(int _force)
{
	return static_cast<int>(enemies[_force].size());
}

int BaseGame::GetFriendCount(int _force)
{
	return static_cast<int>(friends[_force].size());
}

Section_ptr BaseGame::GetSectionData(int _section_id)
{
	for(int force = 0; force < MAX_FORCES; force++)
	{
		BOOST_FOREACH(Core_ptr core, ships_[force])
		{
			if(core->GetSectionID() == _section_id)
				return core;
		}
	}
	return NULL;
}


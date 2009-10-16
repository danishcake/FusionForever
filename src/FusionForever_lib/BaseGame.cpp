#include "StdAfx.h"
#include "BaseGame.h"
#include "LuaChallenge.h"
#include "LuaAI.h"
#include "Decoration.h"
#include "KeyboardAI.h"
#include "SoundManager.h"


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
	luaL_openlibs(luaVM_);
	challenge_ = new LuaChallenge(luaVM_, _challenge_filename, this);
	Camera::Instance().SetupCamera(); //Clears the CameraLevel
	Camera::Instance().SetWidth(static_cast<float>(Camera::Instance().GetWindowWidth())); //Zoom 1:1
	Camera::Instance().SetCentre(0, 0, CameraLevel::Human);
	Camera::Instance().SetFocus(0, 0, CameraLevel::Human);
	running_state_ = GameState::Running;
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

	Camera::Instance().SetCentre(0, 0, CameraLevel::None);
	lua_close(luaVM_);
}

void BaseGame::Draw()
{
//	gas_clouds_.DrawGasClouds(Vector3f(Camera::Instance().GetCentreX(), Camera::Instance().GetCentreY(), 0));
	gas_clouds_.DrawGasClouds(Vector3f(Camera::Instance().GetFocusX(), Camera::Instance().GetFocusY(), 0));
	starfield_.DrawStarfield(Vector3f(Camera::Instance().GetFocusX(), Camera::Instance().GetFocusY(), 0));
	radar_.Draw();
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

int BaseGame::Tick(float _timespan, GameGUI& _gui)
{
	//Tick scripts
	ChallengeState::Enum state = challenge_->Tick(_timespan);
	Camera::Instance().TickCamera(_timespan);

	if(running_state_ == GameState::Running)
	{
		std::vector<Decoration_ptr> decoration_spawn;
		std::vector<Section_ptr> filtered;
		filtered.reserve(50);
		decoration_spawn.reserve(20);

		//Calculate enemies and friends of each force
		for(int force = 0; force < MAX_FORCES; force++)
		{
			enemies[force].clear();
			friends[force].clear();
			for(int other_force = 0; other_force < MAX_FORCES; other_force++)
			{
				if(hostility_[force][other_force] == Hostility::Hostile)
					enemies[force].insert(enemies[force].end(),ships_[other_force].begin(), ships_[other_force].end());
				if(hostility_[force][other_force] == Hostility::Friendly)
					friends[force].insert(friends[force].end(),ships_[other_force].begin(), ships_[other_force].end());
			}
		}
		/* Update the radar */
		radar_.Update(enemies[0], friends[0],_timespan);

		GridCollisionManager::ClearStatic();
		for(int force = 0; force < MAX_FORCES; force++)
		{
			collision_managers_[force].Clear();
		}

		//Tick ships
		const Matrix4f identity = Matrix4f();
		std::vector<Projectile_ptr> projectile_spawn;
		projectile_spawn.reserve(50);
		std::vector<std::vector<Section_ptr>> shields(MAX_FORCES, std::vector<Section_ptr>(8));

		for(int force = 0; force < MAX_FORCES; force++)
		{
			projectile_spawn.clear();
			shields[force].clear();
			BOOST_FOREACH(Core_ptr core, ships_[force])
			{
				core->Tick(_timespan, projectile_spawn, decoration_spawn, identity, friends[force], enemies[force], &collision_managers_[force]);
				core->CollectShields(shields[force]);
			}
			//Add spawned projectiles
			projectiles_[force].insert(projectiles_[force].end(), projectile_spawn.begin(), projectile_spawn.end());
		}

		//Tick projectiles
		for(int force = 0; force < MAX_FORCES; force++)
		{
			BOOST_FOREACH(Projectile_ptr projectile, projectiles_[force])
			{
				projectile->Tick(_timespan, decoration_spawn, identity);
			}
		}

		//Collide projectiles with shields
		for(int force = 0; force < MAX_FORCES; force++)
		{
			for(int other_force = 0; other_force < MAX_FORCES; other_force++)
			{
				if(force != other_force && hostility_[force][other_force] == Hostility::Hostile)
				{
					BOOST_FOREACH(Section_ptr shield, shields[other_force])
					{
						BOOST_FOREACH(Projectile_ptr projectile, projectiles_[force])
						{
							if(Collisions2f::CirclesIntersect(projectile->GetPosition(), projectile->GetRadius(), shield->GetGlobalPosition(), shield->GetShieldRadius()) &&
								shield->GetShieldHealth() > 0)
							{
								shield->TakeDamage(projectile->GetDamage(), shield->GetRoot()->GetSectionID());
								projectile->SetLifetime(-10000);
							}
						}
					}
				}
			}
		}


		//Collide projectiles with ships
		for(int force = 0; force < MAX_FORCES; force++)
		{
			projectile_spawn.clear();
			BOOST_FOREACH(Projectile_ptr projectile, projectiles_[force])
			{
				if(projectile->GetLifetime() > 0)
				{
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
				}
				if(projectile->GetLifetime()<=0)
				{
					projectile->Hit(decoration_spawn, projectile_spawn);
				}
			}
			projectiles_[force].insert(projectiles_[force].end(), projectile_spawn.begin(), projectile_spawn.end());
		}

		//Ship-ship collisions
		size_t check_count = 0;
		for(int force_a = 0; force_a < MAX_FORCES; force_a++)
		{
			for(int force_b = force_a+1; force_b < MAX_FORCES; force_b++)
			{
				if(hostility_[force_a][force_b] == Hostility::Friendly)
					continue;
				for(int x = 0; x < GRID_SECTIONS; x++)
				{
					for(int y = 0; y < GRID_SECTIONS; y++)
					{
						std::vector<Section_ptr>& sections_a = collision_managers_[force_a].GetExactAtCoordinate(x, y);
						std::vector<Section_ptr>& sections_b = collision_managers_[force_b].GetAtCoordinate(x, y);
						if(sections_a.size() > 0 && sections_b.size() > 0)
						{
							BOOST_FOREACH(Section_ptr a, sections_a)
							{
								BOOST_FOREACH(Section_ptr b, sections_b)
								{
									if(Collisions2f::CirclesIntersect(a->GetGlobalPosition(), a->GetRadius(), b->GetGlobalPosition(), b->GetRadius()))
									{
										//Circle-circle collisions seem to be close enough. 
										//If a and b overlap then reduce both healths by minimum of the two healths
										float lowest_health = a->GetHealth() < b->GetHealth() ? a->GetHealth() : b->GetHealth();
										a->TakeDamage(lowest_health, b->GetRoot()->GetSectionID());
										b->TakeDamage(lowest_health, a->GetRoot()->GetSectionID());
										SoundManager::Instance().PlaySample("Crash1.wav");
									}
								}
							}
							check_count += sections_a.size() * sections_b.size();
						}
					}
				}
			}
		}

		//Get dead sections deathspawn
		for(int force = 0; force < MAX_FORCES; force++)
		{
			BOOST_FOREACH(Core_ptr core, ships_[force])
			{
				if(core->GetHealth() <= 0)
				{
					core->GetDeathSpawn(decoration_spawn);
					challenge_->CallDeathFunction(core->GetDeathFunctionReference());
				}
			}
		}
		//Add spawned decorations 
		decorations_.insert(decorations_.end(), decoration_spawn.begin(), decoration_spawn.end());
		decoration_spawn.clear();
		//Tick decorations
		BOOST_FOREACH(Decoration_ptr decoration, decorations_)
		{
			decoration->Tick(_timespan, identity, decoration_spawn);
		}
		//Add spawned decorations 
		decorations_.insert(decorations_.end(), decoration_spawn.begin(), decoration_spawn.end());
		//Tick just the spawned decorations. Lets not worry about generational issues
		BOOST_FOREACH(Decoration_ptr decoration, decoration_spawn)
		{
			decoration->Tick(_timespan, identity, decoration_spawn);
		}


		//Remove the dead
		decorations_.erase(std::remove_if(decorations_.begin(), decorations_.end(), Decoration::IsRemovable), decorations_.end());
		for(int force = 0; force < MAX_FORCES; force++)
		{
			projectiles_[force].erase(std::remove_if(projectiles_[force].begin(), projectiles_[force].end(), Projectile::IsProjectileRemovable), projectiles_[force].end());
			ships_[force].erase(std::remove_if(ships_[force].begin(), ships_[force].end(), Section::IsRemovable), ships_[force].end());
		}
	} else
	{
		
	}

	_gui = gui_; //Copy in messages and counter data
	//Update the health/energy bars displayed
	if(KeyboardAI::GetPlayerIDs().size() > 0)
	{
		int player_id = KeyboardAI::GetPlayerIDs()[0];
		Core_ptr core = static_cast<Core_ptr>(GetSectionData(player_id));
		if(core)
		{
		_gui.ship_health_fraction = 1.0f - (core->GetTotalDamage() / core->GetTotalHealth());
		_gui.core_health_fraction = (core->GetHealth() / core->GetMaxHealth());
		_gui.energy_fraction = core->GetEnergy().GetValue() / core->GetEnergy().GetMaxValue();
		_gui.render_bars = true;
		}
	} else
		_gui.render_bars = false;
	
	//_gui.new_messages_.insert(_gui.new_messages_.end(), gui_.new_messages_.begin(), gui_.new_messages_.end());
	gui_.new_messages_.clear();

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

void BaseGame::Resume()
{
	if(running_state_ == GameState::Running)
	{
		Logger::DiagnosticOut() << "Warning: Script resumed an already running game - error in challenge script?\n";
	}
	running_state_ = GameState::Running;
}

void BaseGame::Pause()
{
	if(running_state_ == GameState::Paused)
	{
		Logger::DiagnosticOut() << "Warning: Script paused an already paused game - error in challenge script?\n";
	}
	running_state_ = GameState::Paused;
}

Section_ptr BaseGame::GetCoreData(int _section_id)
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

Section_ptr BaseGame::GetSectionData(int _section_id)
{
	for(int force = 0; force < MAX_FORCES; force++)
	{
		BOOST_FOREACH(Core_ptr core, ships_[force])
		{
			Section* section = core->GetSectionByID(_section_id);
			if(section)
				return section;
		}
	}
	return NULL;
}

std::vector<Core*> BaseGame::GetShipsInArea(Vector3f _position, float _radius)
{
	std::vector<Core*> cores;
	cores.reserve(10);
	for(int force = 0; force < MAX_FORCES; force++)
	{
		BOOST_FOREACH(Core_ptr core, ships_[force])
		{
			if(Collisions2f::PointInCircle(core->GetPosition(), _position, _radius))
			{
				cores.push_back(core);
			}
		}
	}
	return cores;
}


void BaseGame::SetHostility(int _force_a, int _force_b, bool _hostile)
{
	hostility_[_force_a][_force_b] = _hostile ? Hostility::Hostile : Hostility::Friendly;
}

void BaseGame::DisplayMessage(std::string _message, float _time)
{
	ScreenText st;
	st.text = _message;
	st.time = _time;
	gui_.new_messages_.push_back(st);
}

void BaseGame::SetCounter(int _counter_id, int _value, int _max, bool _visible)
{
	switch(_counter_id)
	{
	case 1:
		gui_.counter_A_visible = _visible;
		gui_.counter_A_value= _value;
		gui_.counter_A_max = _max;
		break;
	case 2:
		gui_.counter_B_visible = _visible;
		gui_.counter_B_value= _value;
		gui_.counter_B_max = _max;
		break;
	case 3:
		gui_.counter_C_visible = _visible;
		gui_.counter_C_value= _value;
		gui_.counter_C_max = _max;
		break;
	}
}

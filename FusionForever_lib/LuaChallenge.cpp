#include "StdAfx.h"
#include "LuaChallenge.h"
#include "KeyboardAI.h"
#include "LuaAI.h"
#include "BaseGame.h"
#include <boost/algorithm/string.hpp>
#include "LuaTimeout.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int LuaChallenge::l_SpawnShip(lua_State* _luaVM)
{
	//SpawnShip(self.ship_pointer, ship_name, force, position.x, position.y, angle, ai_script, health_scale)
	if(!(lua_gettop(_luaVM) == 8))
	{
		luaL_error(_luaVM, "SpawnShip must be called with 8 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -8)));
	assert(challenge);
	
	const char* ship_name = lua_tostring(_luaVM, -7);
	assert(ship_name);

	
	int force = static_cast<int>(lua_tointeger(_luaVM, -6));
	assert(force >= 0 && force < 8);
	Vector2f position = Vector2f(static_cast<float>(lua_tonumber(_luaVM, -5)), static_cast<float>(lua_tonumber(_luaVM, -4)));
	float angle = static_cast<float>(lua_tonumber(_luaVM, -3));

	const char* ai_script = lua_tostring(_luaVM, -2);
	assert(ai_script);
	
	float health_scale = static_cast<float>(lua_tonumber(_luaVM, -1));

	int ship_id = challenge->SpawnShip(std::string(ship_name), force, position, angle, std::string(ai_script), health_scale, _luaVM);

	lua_pushnumber(_luaVM, ship_id);

	return 1;
}

int LuaChallenge::SpawnShip(std::string _ship_name, int _force, Vector2f _position, float angle, std::string _ai_script, float _health_scale, lua_State* _luaVM)
{
	//Attempt to load ship
	Core_ptr core = Core::CreateCore(_ship_name);
	if(core)
	{
		core->SetPosition(Vector3f(_position.x, _position.y, 0));
		core->SetAngle(angle);
		core->SetColor(force_colors_[_force]);
		core->ScaleHealth(_health_scale);
		
		//Set the AI - either KeyboardAI or a LuaAI
		std::transform(_ai_script.begin(), _ai_script.end(), _ai_script.begin(), toupper);
		BaseAI* ai = NULL;
		if(boost::iequals(_ai_script, std::string("KEYBOARDAI")))
		{
			ai = new KeyboardAI();
		} else
		{
			ai = LuaAI::FromScript(_ai_script, luaVM_);
			if(!ai)
			{
				delete core; 
				luaL_error(_luaVM, (std::string("Error loading AI:") + _ai_script).c_str());
			}
		}
		if(ai != NULL)
		{
			core->OverrideAI(ai);
		}
		game_->AddShip(core, _force);
	} else
	{
		luaL_error(_luaVM, (std::string("Unable to load Ship") + _ship_name).c_str());
	}
	return core->GetSectionID();
}

int LuaChallenge::l_GetShipsInArea(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 4))
	{
		luaL_error(_luaVM, "SpawnShip must be called with 4 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -4)));
	assert(challenge);
	float x = static_cast<float>(lua_tonumber(_luaVM, -3));
	float y = static_cast<float>(lua_tonumber(_luaVM, -2));
	float r = static_cast<float>(lua_tonumber(_luaVM, -1));

	lua_createtable(_luaVM, 0, 0); //Stack contains an empty table

	//Iterate over forces and build a table with index of all ships within circle
	int ship_index = 0;
	std::vector<Core_ptr> cores = challenge->GetShipsInArea(Vector3f(x, y, 0), r);
	BOOST_FOREACH(Core_ptr core, cores)
	{
		lua_pushinteger(_luaVM, ship_index);
		lua_pushinteger(_luaVM, core->GetSectionID());
		lua_settable(_luaVM, -3);
		ship_index++;
	}
	return 1;
}

std::vector<Core_ptr> LuaChallenge::GetShipsInArea(Vector3f _position, float _radius)
{
	return game_->GetShipsInArea(_position, _radius);
}


int LuaChallenge::l_KillShip(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 2))
	{
		luaL_error(_luaVM, "SpawnShip must be called with 2 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -2)));
	assert(challenge);

	int ship_id_target = static_cast<int>(lua_tointeger(_luaVM, -1));
	Core_ptr target = challenge->GetShipData(ship_id_target);
	target->TakeDamage(target->GetMaxHealth(), -1); //Anonymous damage
	return 0;
}

int LuaChallenge::l_luaError(lua_State* _luaVM)
{
	Logger::ErrorOut() << lua_tostring(_luaVM, -1) << "\n";
	
	return 0;
}

int LuaChallenge::l_SetHostility(lua_State* _luaVM)
{
	if(lua_gettop(_luaVM) != 4)
	{
		luaL_error(_luaVM, "SetHostility must be called with 4 parameters");
	}
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -4)));
	assert(challenge);

	int forceA = static_cast<int>(lua_tonumber(_luaVM, -3));
	int forceB = static_cast<int>(lua_tonumber(_luaVM, -2));

	if(forceA < 0 || forceA > MAX_FORCES)
		luaL_error(_luaVM, "ForceA should be between 0 and %d", MAX_FORCES);
	if(forceB < 0 || forceB > MAX_FORCES)
		luaL_error(_luaVM, "ForceB should be between 0 and %d", MAX_FORCES);

	bool hostile = (lua_toboolean(_luaVM, -1) != 0);

	challenge->SetHostility(forceA, forceB, hostile);
	return 0;
}

void LuaChallenge::SetHostility(int _force_a, int _force_b, bool _hostility)
{
	game_->SetHostility(_force_a, _force_b, _hostility);
}

int LuaChallenge::l_SetDeathFunction(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 3))
	{
		luaL_error(_luaVM, "SetDeathFunction must be called with 3 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -3)));
	assert(challenge);

	int ship_id = static_cast<int>(lua_tointeger(_luaVM, -2));
	int death_function_reference = luaL_ref(_luaVM, LUA_REGISTRYINDEX);
	Core_ptr core = static_cast<Core_ptr>(challenge->GetShipData(ship_id));
	core->SetDeathFunctionReference(death_function_reference);

	return 0;
}

int LuaChallenge::l_Victory(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 1))
	{
		luaL_error(_luaVM, "Victory must be called with just the ChallengePointer");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -1)));
	assert(challenge);
	challenge->DeclareVictory();
	return 0;
}

int LuaChallenge::l_Defeat(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 1))
	{
		luaL_error(_luaVM, "Defeat must be called with just the ChallengePointer");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -1)));
	assert(challenge);
	challenge->DeclareDefeat();
	return 0;
}

int LuaChallenge::l_Draw(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 1))
	{
		luaL_error(_luaVM, "Draw must be called with just the ChallengePointer");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -1)));
	assert(challenge);
	challenge->DeclareDraw();
	return 0;
}

int LuaChallenge::l_ReturnToEditor(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 1))
	{
		luaL_error(_luaVM, "ReturnToEditor must be called with just the ChallengePointer");
	}
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -1)));
	assert(challenge);
	challenge->ReturnToEditor();
	return 0;
}

int LuaChallenge::l_SetShipTarget(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 3))
	{
		luaL_error(_luaVM, "GetShipData must be called with 3 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -3)));
	assert(challenge);
	
	int ship_id_target = static_cast<int>(lua_tointeger(_luaVM, -1));
	Core_ptr target = challenge->GetShipData(ship_id_target);
	
	int ship_id_core = static_cast<int>(lua_tointeger(_luaVM, -2));
	Core_ptr core = challenge->GetShipData(ship_id_core);

	if(core && target)
	{
		core->GetAI()->SpecifyTarget(target);
		Logger::DiagnosticOut() << "Making " << ship_id_core << " target " << ship_id_target << "\n";
	}
	if(!core)
		Logger::ErrorOut() << "Unable to find target with ID" << ship_id_target << "\n";
	if(!target)
		Logger::ErrorOut() << "Unable to find target with ID" << ship_id_core << "\n";

	return 0;
}

int LuaChallenge::l_GetShipData(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 2))
	{
		luaL_error(_luaVM, "GetShipData must be called with 2 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -2)));
	assert(challenge);

	int ship_id = static_cast<int>(lua_tointeger(_luaVM, -1));

	Core_ptr core = challenge->GetShipData(ship_id);
	if(core != NULL)
	{
		lua_pushnumber(_luaVM, core->GetPosition().x);
		lua_pushnumber(_luaVM, core->GetPosition().y);
		lua_pushnumber(_luaVM, core->GetAngle());
		lua_pushboolean(_luaVM, true);
	} else
	{
		lua_pushnumber(_luaVM, 0);
		lua_pushnumber(_luaVM, 0);
		lua_pushnumber(_luaVM, 0);
		lua_pushboolean(_luaVM, false);
	}

	return 4;
}

Core* LuaChallenge::GetShipData(int _ship_id)
{
	return static_cast<Core_ptr>(game_->GetSectionData(_ship_id));
}

int LuaChallenge::l_DisplayMessage(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 3))
	{
		luaL_error(_luaVM, "DisplayMessage must be called with 3 parameters");
	}
	float time = static_cast<float>(lua_tonumber(_luaVM, -1));
	std::string text = lua_tostring(_luaVM, -2);
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -3)));
	assert(challenge);

	challenge->DisplayMessage(text, time);

	return 0;
}

void LuaChallenge::DisplayMessage(std::string _message, float _time)
{
	game_->DisplayMessage(_message, _time + 0.5f);
}

int LuaChallenge::l_SetCounter(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 5))
	{
		luaL_error(_luaVM, "SetCounter must be called with 5 parameters");
	}
	int max = static_cast<int>(lua_tointeger(_luaVM, -1));
	int value = static_cast<int>(lua_tointeger(_luaVM, -2));
	bool visible = (lua_toboolean(_luaVM, -3) != 0);
	int counter_id = static_cast<int>(lua_tointeger(_luaVM, -4));
	if(counter_id < 1 || counter_id > 3)
	{
		luaL_error(_luaVM, "Counter must be between 1 and 3");
		return 0;
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -5)));
	assert(challenge);

	challenge->SetCounter(counter_id, value, max, visible);
	return 0;
}

void LuaChallenge::SetCounter(int _counter, int _value, int _max, bool _visible)
{
	game_->SetCounter(_counter, _value, _max, _visible);
}

LuaChallenge::LuaChallenge(lua_State* _luaVM, std::string _challenge, BaseGame* _game) : 
	luaVM_(_luaVM), challenge_(_challenge), game_(_game)
{
	lua_register(_luaVM, "SpawnShip", l_SpawnShip);
	lua_register(_luaVM, "Victory", l_Victory);
	lua_register(_luaVM, "Defeat", l_Defeat);
	lua_register(_luaVM, "Draw", l_Draw);
	lua_register(_luaVM, "ReturnToEditor", l_ReturnToEditor);
	lua_register(_luaVM, "GetShipData", l_GetShipData);
	lua_register(_luaVM, "GetShipsInArea", l_GetShipsInArea);
	lua_register(_luaVM, "_ALERT", l_luaError);
	lua_register(_luaVM, "SetDeathFunction", l_SetDeathFunction);
	lua_register(_luaVM, "SetShipTarget", l_SetShipTarget);
	lua_register(_luaVM, "SetHostility", l_SetHostility);
	lua_register(_luaVM, "DisplayMessage", l_DisplayMessage);
	lua_register(_luaVM, "SetCounter", l_SetCounter);
	lua_register(_luaVM, "KillShip", l_KillShip);

	state_ = ChallengeState::NotStarted;

	force_colors_[0] = GLColor(210, 0  , 0  );
	force_colors_[1] = GLColor(0  , 210, 0  );
	force_colors_[2] = GLColor(0  , 0  , 210);
	force_colors_[3] = GLColor(210, 210, 0  );
	force_colors_[4] = GLColor(0  , 210, 210);
	force_colors_[5] = GLColor(210, 0  , 210);
	force_colors_[6] = GLColor(50 , 50 , 50 );
	force_colors_[7] = GLColor(205, 205, 205);
	
	sum_time_ = 0;

	chunk_reference_ = 0;
	environment_reference_ = 0;
	coroutine_reference_ = 0;

	monitor_thread_ = new LuaTimeout("Challenge script has hung after 5 seconds without yield\n", 5.0f);


	assert(lua_gettop(_luaVM) == 0);
	//Now load the challenge chunk
	if(loadChallenge() == false)
	{
		assert(lua_gettop(_luaVM) == 0); 
		state_ = ChallengeState::LoadError;
		Logger::ErrorOut() << "Unable to load challenge script: " << challenge_ << "\n";
	} else
	{
		assert(lua_gettop(_luaVM) == 0);
		if(createEnvironmentAndCoroutine() == false)
		{
			state_ = ChallengeState::LoadError;
			Logger::ErrorOut() << "Unable to create environment. Is Challenge_sandbox broken?\n";
		} else
		{
			state_ = ChallengeState::Running;
		}
	}
}

LuaChallenge::~LuaChallenge(void)
{
	if(coroutine_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, coroutine_reference_);
	if(environment_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, environment_reference_);
	if(chunk_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, chunk_reference_);
	delete monitor_thread_;
}

bool LuaChallenge::loadChallenge()
{
	int load_result = luaL_loadfile(luaVM_, (std::string("Scripts/Challenges/") + challenge_).c_str());
	if(load_result==0)
	{
		chunk_reference_ = luaL_ref(luaVM_, LUA_REGISTRYINDEX);
		Logger::ErrorOut() << "Challenge loaded OK\n";
		return true;
	} else
	{
		std::string error_string = lua_tostring(luaVM_, -1);
		lua_pop(luaVM_, 1);
		Logger::ErrorOut() << "Load Challenge script error: " << error_string << "\n";
		return false;
	}
}

bool LuaChallenge::createEnvironmentAndCoroutine()
{
	lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, chunk_reference_); //Gets the function ready for setfenv

	/* Sandbox a file*/
	int env_load_error = luaL_loadfile(luaVM_, "Challenge_sandbox.lua");
	//Either have error message or chunk on stack
	if(env_load_error != 0 )
	{
		Logger::ErrorOut() << lua_tostring(luaVM_, -1) << "\n";
		lua_pop(luaVM_, 2);
		return false;
	}

	//The environment has a reference to this class in it...
	lua_pushlightuserdata(luaVM_, this);
	
	int env_run_error = lua_pcall(luaVM_, 1, 1, NULL);
	if(env_run_error != 0 )
	{
		Logger::ErrorOut() << lua_tostring(luaVM_, -1) << "\n";
		lua_pop(luaVM_, 2);
		return false;
	}
	//Should now have a reference to the environment on stack
	environment_reference_ = luaL_ref(luaVM_, LUA_REGISTRYINDEX);
	lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, environment_reference_); //Gets the environment back

	lua_setfenv(luaVM_, -2);
	lua_pop(luaVM_, 1); //Pops chunk from stack
	assert(lua_gettop(luaVM_) == 0);


	lua_State* coroutine = lua_newthread(luaVM_);
	coroutine_reference_= luaL_ref(luaVM_, LUA_REGISTRYINDEX); //Stores the coroutine reference. (also pops it :( )

	lua_rawgeti(coroutine, LUA_REGISTRYINDEX, chunk_reference_); //Gets the function for coroutine.

	return true; //Stack coroutine now contains the chunk to be executed, and is stored at coroutine_reference_
}

ChallengeState::Enum LuaChallenge::Tick(float _timespan)
{
	if(state_ == ChallengeState::Running)
	{
		sum_time_ += _timespan;
		
		// obtain coroutine
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, coroutine_reference_);
		lua_State* thread = lua_tothread(luaVM_, -1);
		lua_pop(luaVM_, 1); //Stack = empty (Checked)

		// Obtain Environment, then update info
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, environment_reference_);		//Stack = env
		lua_pushstring(luaVM_, "challenge");								//Stack = env-"challenge"
		lua_gettable(luaVM_, -2); 											//Stack = env-challenge (Checked)

		lua_pushstring(luaVM_, "time");										//Stack = env-challenge-"time"
		lua_pushnumber(luaVM_, sum_time_);									//Stack = env-challenge-"time"-#sum_time
		lua_settable(luaVM_, -3);											//Stack = env-challenge (Checked)

		lua_pushstring(luaVM_, "enemy_count");								//Stack = env-challenge-"enemy_count"
		lua_gettable(luaVM_, -2); 											//Stack = env-challenge-enemy_count
		for(int force = 0; force <MAX_FORCES; force++)
		{
			lua_pushinteger(luaVM_, force);									//Stack = env-challenge-enemy_count-#force
			lua_pushinteger(luaVM_, game_->GetEnemyCount(force));			//Stack = env-challenge-enemy_count-#force-#enemy_count
			lua_settable(luaVM_, -3);										//Stack = env-challenge-enemy_count
		}
		lua_pop(luaVM_, 1);													//Stack = env-challenge
		lua_pushstring(luaVM_, "force_count");								//Stack = env-challenge-"force_count"
		lua_gettable(luaVM_, -2);											//Stack = env-challenge-force_count
		for(int force = 0; force <MAX_FORCES; force++)
		{
			lua_pushinteger(luaVM_, force);									//Stack = env-challenge-force_count-#force
			lua_pushinteger(luaVM_, game_->GetForceCount(force));			//Stack = env-challenge-force_count-#force-#force_count
			lua_settable(luaVM_, -3);										//Stack = env-challenge-force_count
		}
		lua_pop(luaVM_, 1);													//Stack = env-challenge
		lua_pushstring(luaVM_, "friend_count");								//Stack = env-challenge-"friend_count"
		lua_gettable(luaVM_, -2);											//Stack = env-challenge-friend_count
		for(int force = 0; force <MAX_FORCES; force++)
		{
			lua_pushinteger(luaVM_, force);									//Stack = env-challenge-friend_count-#force
			lua_pushinteger(luaVM_, game_->GetFriendCount(force));			//Stack = env-challenge-friend_count-#force-#friend_count
			lua_settable(luaVM_, -3);										//Stack = env-challenge-friend_count
		}

		lua_pop(luaVM_, 3);													//Stack = empty

		assert(lua_gettop(luaVM_) == 0);

		int resume_result = monitor_thread_->SafeResume(thread);;
		if(resume_result == LUA_YIELD)
		{
			//A normal yield
		} else if(resume_result == 0)
		{
			//The script is over - if no final state is reported then call it a draw
			if(state_ == ChallengeState::Running)
				state_= ChallengeState::Draw;
		} else
		{
			//The script encountered a runtime error, log it
			state_ = ChallengeState::RunError;
			Logger::ErrorOut() << lua_tostring(thread, -1) << "\n";
			game_->DisplayMessage(lua_tostring(thread, -1), 10);
			lua_pop(thread, 1); //Pops the error message
			DeclareDraw();
			assert(lua_gettop(thread) == 0);
		}
	}
	
	assert(lua_gettop(luaVM_) == 0);
	return state_;
}

void LuaChallenge::CallDeathFunction(int _death_function_refence)
{
	if(_death_function_refence != 0)
	{
		// obtain death function
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, _death_function_refence);
		//This function should have been created in the correct environment, and thus inheritted it.
		int run_result = lua_pcall(luaVM_, 0, 0, 0);
		if(run_result != 0)
		{
			Logger::ErrorOut() << "Error running death function" << boost::lexical_cast<std::string, int>(_death_function_refence) << "\n";
			Logger::ErrorOut() << "Error:" << lua_tostring(luaVM_, -1) << "\n";
			lua_pop(luaVM_, 1);
		}
		//Dispose of the function reference
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, _death_function_refence);
	}
}
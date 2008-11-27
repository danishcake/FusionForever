#include "StdAfx.h"
#include "LuaChallenge.h"
#include "KeyboardAI.h"
#include "LuaAI.h"
#include "BaseGame.h"
#include <boost/algorithm/string.hpp>

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
		//if(std::equal(_ai_script.begin(), _ai_script.begin(), std::string("KEYBOARDAI").begin()))
		//if(std::equal(_ai_script.begin(), _ai_script.end(), "KEYBOARDAI"))
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

int LuaChallenge::l_luaError(lua_State* _luaVM)
{
	Logger::LogError(lua_tostring(_luaVM, -1));
	
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

int LuaChallenge::l_GetShipData(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 2))
	{
		luaL_error(_luaVM, "GetShipData must be called with 2 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -2)));
	assert(challenge);

	int ship_id = static_cast<int>(lua_tointeger(_luaVM, -1));

	Section_ptr core = challenge->GetShipData(ship_id);
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

Section* LuaChallenge::GetShipData(int _ship_id)
{
	return game_->GetSectionData(_ship_id);
}

LuaChallenge::LuaChallenge(lua_State* _luaVM, std::string _challenge, BaseGame* _game) : 
	luaVM_(_luaVM), challenge_(_challenge), game_(_game)
{
	lua_register(_luaVM, "SpawnShip", l_SpawnShip);
	lua_register(_luaVM, "Victory", l_Victory);
	lua_register(_luaVM, "Defeat", l_Defeat);
	lua_register(_luaVM, "Draw", l_Draw);
	lua_register(_luaVM, "GetShipData", l_GetShipData);
	lua_register(_luaVM, "_ALERT", l_luaError);
	

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

	assert(lua_gettop(_luaVM) == 0);
	//Now load the challenge chunk
	if(loadChallenge() == false)
	{
		assert(lua_gettop(_luaVM) == 0); 
		state_ = ChallengeState::LoadError;
		Logger::LogError(std::string("Unable to load challenge script: ") + challenge_);
	} else
	{
		assert(lua_gettop(_luaVM) == 0);
		if(createEnvironmentAndCoroutine() == false)
		{
			state_ = ChallengeState::LoadError;
			Logger::LogError(std::string("Unable to create environment. Is Challenge_sandbox broken?"));
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
}

bool LuaChallenge::loadChallenge()
{
	int load_result = luaL_loadfile(luaVM_, (std::string("Scripts/Challenges/") + challenge_).c_str());
	if(load_result==0)
	{
		chunk_reference_ = luaL_ref(luaVM_, LUA_REGISTRYINDEX);
		Logger::Instance() << "Challenge loaded OK\n";
		return true;
	} else
	{
		std::string error_string = lua_tostring(luaVM_, -1);
		lua_pop(luaVM_, 1);
		Logger::Instance() << "Load Challenge script error: " << error_string << "\n";
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
		Logger::Instance() << lua_tostring(luaVM_, -1);
		lua_pop(luaVM_, 2);
		return false;
	}

	//The environment has a reference to this class in it...
	lua_pushlightuserdata(luaVM_, this);
	
	int env_run_error = lua_pcall(luaVM_, 1, 1, NULL);
	if(env_run_error != 0 )
	{
		Logger::Instance() << lua_tostring(luaVM_, -1);
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

		int resume_result = lua_resume(thread, 0);
		if(resume_result == LUA_YIELD)
		{
			//Logger::Instance() << "Coroutine yielded OK\n";
		} else if(resume_result == 0)
		{
			//Logger::Instance() << "Coroutine finished without errors\n";
			state_= ChallengeState::Draw;
		} else
		{
			state_ = ChallengeState::RunError;
			Logger::Instance() << lua_tostring(thread, -1) << "\n";
		}
	}
	
	assert(lua_gettop(luaVM_) == 0);
	return state_;
}
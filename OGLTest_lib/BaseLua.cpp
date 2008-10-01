#include "StdAfx.h"
#include "BaseLua.h"

#include "SquareCore.h"
#include "TinyCore.h"
#include "Blaster.h"
#include "HomingMissileLauncher.h"
#include "Swarmer.h"
#include "HeatBeamGun.h"
#include "SpinningJoint.h"
#include "JointAngles.h"
#include "JointTracker.h"
#include "LuaSection.h"
#include "RotatingAI.h"
#include "KeyboardAI.h"
#include "SimpleAI.h"
#include "LuaAI.h"

#include <string>
#include <algorithm>
#include <sstream>
#include "BaseGame.h"

static BaseLua* last_instantiation = NULL;
lua_State* luaVM;

static enum SectionType
{
	unknown_key,
	st_SquareCore,
	st_TinyCore,
	st_Blaster,
	st_HomingMissileLauncher,
	st_Swarmer,
	st_HeatBeam,
	st_SpinningJoint,
	st_JointAngles,
	st_TrackerJoint,
	ai_RotatingAI,
	ai_KeyboardAI,
	ai_SimpleAI
};

static std::map<std::string, SectionType> SectionMap;

static void InitialiseMap()
{
	SectionMap["KEYBOARDAI"] = ai_KeyboardAI;
	SectionMap["ROTATINGAI"] = ai_RotatingAI;
	SectionMap["SIMPLEAI"] = ai_SimpleAI;

	SectionMap["SQUARECORE"] = st_SquareCore;
	SectionMap["BLASTER"] = st_Blaster;
	SectionMap["HEATBEAM"] = st_HeatBeam;
	SectionMap["SPINNINGJOINT"] = st_SpinningJoint;
	SectionMap["JOINTANGLES"] = st_JointAngles;
	SectionMap["HOMINGMISSILELAUNCHER"] = st_HomingMissileLauncher;
	SectionMap["TINYCORE"] = st_TinyCore;
	SectionMap["TRACKERJOINT"] = st_TrackerJoint;
	SectionMap["SWARMER"] = st_Swarmer;
}

static int l_add_to_force(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if(lua_gettop(luaVM) == 1 &&(lua_isnumber(luaVM, -1)))
	{
		int force = static_cast<int>(lua_tonumber(luaVM, -1));
		if(force >= 0 && force < MAX_FORCES)
			last_instantiation->AddToForce(force);
		else
			luaL_error(luaVM, "AddToForce force parameter must be in range 0 to %d",  MAX_FORCES-1);
	}
	else
	{
		luaL_error(luaVM, "AddToForce must be called with 1 numeric parameter");
	}
	return 0;
}

static int l_set_angle(lua_State * luaVM)
{
	assert(last_instantiation!=NULL);
	if((lua_gettop(luaVM) == 1) && (lua_isnumber(luaVM, -1)))
	{
		last_instantiation->SetAngle((float)lua_tonumber(luaVM, 1));
	}
	else
	{
		luaL_error(luaVM, "SetAngle must be called with 1 numeric parameter");
	}
	return 0;
}
static int l_set_position(lua_State * luaVM)
{
	assert(last_instantiation!=NULL);
	if((lua_gettop(luaVM) == 2) && (lua_isnumber(luaVM, -1)) && (lua_isnumber(luaVM, -2)))
	{
		last_instantiation->SetPosition((float)lua_tonumber(luaVM,1), (float)lua_tonumber(luaVM,2));
	}
	else
	{
		luaL_error(luaVM, "SetPosition must be called with 2 numeric parameters");
	}
	return 0;
}

static int l_load_ship(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if((lua_gettop(luaVM) == 1) && (lua_isstring(luaVM, -1)))
	{
		int ship_id=-1;
		Core_ptr core = Core::CreateCore(lua_tostring(luaVM, -1));
		if(core)
		{
			last_instantiation->SetCore(core);
			ship_id = core->GetSectionID();
		}
		else
		{
			luaL_error(luaVM,"LoadShip failed");
			
		}
		lua_pushnumber(luaVM, ship_id); //Push -1 in case of error, else ship Core section ID
	}
	else
	{
		luaL_error(luaVM, "LoadShip must be called with 1 string parameter");
	}
	return 1;
}

static int l_set_force_color(lua_State* luaVM)
{
	assert(last_instantiation != NULL);
	if((lua_gettop(luaVM) == 3) && (lua_isnumber(luaVM, -1)) &&
																 (lua_isnumber(luaVM, -2)) &&
																 (lua_isnumber(luaVM, -3)))
	{
		int force = static_cast<int>(lua_tonumber(luaVM, 1));
		double r = lua_tonumber(luaVM, 2);
		double g = lua_tonumber(luaVM, 3);
		double b = lua_tonumber(luaVM, 4);
		if((r >= 0.0 && r <= 255) &&
			 (g >= 0.0 && g <= 255) &&
			 (b >= 0.0 && b <= 255) &&
			 (force >=0 && force < MAX_FORCES))
		{
			last_instantiation->SetForceColor(force, GLColor(static_cast<unsigned char>(r),
															 static_cast<unsigned char>(g),
															 static_cast<unsigned char>(b)));
		} else
		{
			luaL_error(luaVM, "SetColor parameters must be in range force(0-%d), color(0 to 255);",MAX_FORCES-1);
		}
	}
	else
	{
		luaL_error(luaVM, "SetColor must be called with 4 numeric parameters, force, red, green, blue");
	}
	return 0;
}


static int l_scale_health(lua_State* luaVM)
{
	assert(last_instantiation != NULL);
	if((lua_gettop(luaVM) == 1) && (lua_isnumber(luaVM, -1)))
	{
		last_instantiation->ScaleHealth(static_cast<float>(lua_tonumber(luaVM, -1)));
	}
	else
	{
		luaL_error(luaVM, "ScaleHealth must be called with 1 numeric parameter");
	}
	return 0;
}

static int l_override_ai(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if(lua_gettop(luaVM) >= 1)
	{
		std::string ai_string = lua_tostring(luaVM, 1);
		std::transform(ai_string.begin(), ai_string.end(), ai_string.begin(), toupper);

		SectionType ai_type = SectionMap[ai_string];
		BaseAI* ai = NULL;
		switch(ai_type)
		{
		case ai_RotatingAI:
			{
				if(lua_gettop(luaVM) == 2)
				{
					float rotRate = (float)lua_tonumber(luaVM, 2);
					ai = new RotatingAI(rotRate);
				}
				else
				{
					luaL_error(luaVM, "SetAI(\"RotatingAI\"... expects a two parameters, the second being the rotation rate");
				}
			}
			break;
		case ai_KeyboardAI:
			if(lua_gettop(luaVM) == 1)
			{
				ai = new KeyboardAI();
			} else
			{
				luaL_error(luaVM, "SetAI(\"KeyboardAI\"... expects a one parameter");
			}
			break;
		case ai_SimpleAI:
			if(lua_gettop(luaVM) == 1)
			{
				ai = new SimpleAI();
			} else
			{
				luaL_error(luaVM, "SetAI(\"SimpleAI\"... expects a one parameter");
			}
			break;
		default:
			ai = LuaAI::FromScript(ai_string, luaVM);
			if(!ai)
				luaL_error(luaVM, (std::string("Unrecognised AI script:") + ai_string).c_str());
			break;
		}
		if(ai != NULL)
			last_instantiation->OverrideAI(ai);
	}
	else
	{
		luaL_error(luaVM, "IsAlive must be called with 1 or more parameters, depending on the AI type specified in the first parameter");
	}
	return 0;
}

static int l_is_alive(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if((lua_gettop(luaVM) == 1) && (lua_isnumber(luaVM, -1)))
	{
		int section_id = static_cast<int>(lua_tonumber(luaVM, -1));
		lua_pushboolean(luaVM, last_instantiation->IsAlive(section_id));
	}
	else
	{
		luaL_error(luaVM, "IsAlive must be called with 1 numeric parameter");
	}
	return 1;
}

static int l_log_error(lua_State* luaVM)
{
	if(lua_gettop(luaVM)==1 && lua_isstring(luaVM, -1))
	{
		Logger::LogError(lua_tostring(luaVM, -1));
	}
	else
	{
		lua_pop(luaVM, 1);
		Logger::LogError("Lua Error: LogError() must be called with a string parameter\n");
	}
	return 0;
}
static int l_log_to_screen(lua_State* luaVM)
{
	if(lua_gettop(luaVM)==1 && lua_isstring(luaVM, -1))
	{
		Logger::LogGameMessage(lua_tostring(luaVM, -1));
	}
	else
	{
		lua_pop(luaVM, 1);
		Logger::LogError("Lua Error: LogToScreen() must be called with a string parameter\n");
	}
	return 0;
}
static int l_log_diagnostic(lua_State* luaVM)
{
	if(lua_gettop(luaVM)==1 && lua_isstring(luaVM, -1))
	{
		Logger::Log(lua_tostring(luaVM, -1));
	}
	else
	{
		lua_pop(luaVM, 1); 
		Logger::LogError("Lua Error: Log() must be called with a string parameter\n");
	}
	return 0;
}

static int l_luaError( lua_State *L )
{
	Logger::LogError(lua_tostring(luaVM, -1));
	lua_pop(L, 1);
	return 0;
}

BaseLua::BaseLua(BaseGame* _game)
{
	force_colors_[0] = GLColor(210, 0  , 0  );
	force_colors_[1] = GLColor(0  , 210, 0  );
	force_colors_[2] = GLColor(0  , 0  , 210);
	force_colors_[3] = GLColor(210, 210, 0  );
	force_colors_[4] = GLColor(0  , 210, 210);
	force_colors_[5] = GLColor(210, 0  , 210);
	force_colors_[6] = GLColor(50 , 50 , 50 );
	force_colors_[7] = GLColor(205, 205, 205);

	if(last_instantiation == NULL)
	{
		InitialiseMap(); //Allows me to use a big switch statement
		luaVM = lua_open();
		lua_vm_ = luaVM;
		luaL_openlibs(luaVM);
		//Register all the lua functions
		lua_register(luaVM, "AddToForce", l_add_to_force);
		lua_register(luaVM, "SetAngle", l_set_angle);
		lua_register(luaVM, "SetPosition", l_set_position);
		lua_register(luaVM, "SetForceColor", l_set_force_color);
		lua_register(luaVM, "LoadShip", l_load_ship);
		lua_register(luaVM, "ScaleHealth", l_scale_health);
		lua_register(luaVM, "SetAI", l_override_ai);
		lua_register(luaVM, "IsAlive", l_is_alive);
		lua_register(luaVM, "LogError", l_log_error);
		lua_register(luaVM, "LogToScreen", l_log_to_screen);
		lua_register(luaVM, "Log", l_log_diagnostic);
		lua_register(luaVM, "_ALERT", l_luaError);
	}
	game_ = _game;
	last_instantiation = this;
	is_script_running_ = false;
	sum_time_ = 0;
}

BaseLua::~BaseLua(void)
{
}

void BaseLua::AddToForce(int _force)
{
	if(core_)
	{
		core_->SetColor(force_colors_[_force]);
		game_->AddShip(core_, _force);
		core_ = NULL;
	}
}

void BaseLua::SetAngle(float _angle)
{
	core_->SetAngle(_angle);
}

void BaseLua::SetPosition(float _x, float _y)
{
	core_->SetPosition(Vector3f(_x, _y, 0));
}

void BaseLua::SetHealth(float _health)
{
	core_->SetMaxHealth(_health);
}

void BaseLua::LoadChallenge(std::string _challenge)
{
	lua_getglobal(luaVM,"Challenge");
	if(!lua_isnil(luaVM, -1))
	{
		lua_pushnil(luaVM);
		lua_setglobal(luaVM, "Challenge");
		Logger::Log("GameLua::LoadChallenge: Wiping previous challenge\n");
	}
	lua_pop(luaVM,1); //Pops challenge from stack
	 
	Logger::Log("GameLua::LoadChallenge: Attempting to load challenge\n");
	is_script_running_ = false;
	int load_result = luaL_loadfile(luaVM, _challenge.c_str());

	if(load_result == LUA_ERRSYNTAX)
	{
		Logger::LogError("GameLua::LoadChallenge: A syntax error occurred while loading\n");
		Logger::LogError(lua_tostring(luaVM, -1));
		lua_pop(luaVM, 1);
	} else if(load_result == LUA_ERRMEM)
	{
		Logger::LogError("GameLua::LoadChallenge: A memory allocation error occurred while loading\n");
		Logger::LogError(lua_tostring(luaVM, -1));
		lua_pop(luaVM, 1);
	} else
	{//Loaded OK. Function ready to run at top of stack.
		int run_result = lua_pcall(luaVM, 0, LUA_MULTRET, 0);
		if(run_result == LUA_ERRRUN)
		{
			Logger::LogError("GameLua::LoadChallenge: A runtime error occurred\n");
			Logger::LogError(lua_tostring(luaVM, -1));
			Logger::LogError(lua_tostring(luaVM, -1));
			lua_pop(luaVM, 1);
		}
		else if(run_result == LUA_ERRMEM)
		{
			Logger::LogError("GameLua::LoadChallenge: A memory allocation error occurred while running\n");
			Logger::LogError(lua_tostring(luaVM, -1));
			lua_pop(luaVM, 1);
		}
		else if(run_result == LUA_ERRERR)
		{
			Logger::Log("GameLua::LoadChallenge: Error handling function error\n");
			Logger::LogError(lua_tostring(luaVM, -1));
			lua_pop(luaVM, 1);
		}
		else
		{//Everything worked OK, script loaded
			 is_script_running_ = true;
			 Logger::Log("GameLua::LoadChallenge: Challenge script loaded and run without problem\n");
		}
	}
}

void BaseLua::Tick(float _timespan)
{
	if(!is_script_running_)
		return;
	sum_time_ += _timespan;

	lua_newtable(luaVM);
	for(int force = 0; force <MAX_FORCES; force++)
	{
		lua_pushinteger(luaVM, force);
		lua_pushinteger(luaVM, game_->GetFriendCount(force));
		lua_settable(luaVM, -3);
	}
	lua_setglobal(luaVM, "FRIEND_COUNT");

	lua_newtable(luaVM);
	for(int force = 0; force <MAX_FORCES; force++)
	{
		lua_pushinteger(luaVM, force);
		lua_pushinteger(luaVM, game_->GetForceCount(force));
		lua_settable(luaVM, -3);
	}
	lua_setglobal(luaVM, "FORCE_COUNT");

	lua_newtable(luaVM);
	for(int force = 0; force <MAX_FORCES; force++)
	{
		lua_pushinteger(luaVM, force);
		lua_pushinteger(luaVM, game_->GetEnemyCount(force));
		lua_settable(luaVM, -3);
	}
	lua_setglobal(luaVM, "ENEMY_COUNT");


	lua_pushnumber(luaVM, sum_time_);
	lua_setglobal(luaVM, "TOTAL_TIME");


	lua_getglobal(luaVM, "Challenge");
	if(lua_istable(luaVM, -1))
	{
		lua_pushstring(luaVM, "EntryPoint");
		lua_gettable(luaVM, -2);
		if(lua_isfunction(luaVM, -1))
		{
			int run_result = lua_pcall(luaVM, 0, LUA_MULTRET, 0); //Should pop EntryPoint and replace with either true or false, then any error codes
			int parameter_count = lua_gettop(luaVM);
			if(run_result == LUA_ERRRUN)
			{//Runtime error, should report and abandon
				Logger::LogError("GameLua::Tick: A runtime error occurred:\n");
				if(lua_isstring(luaVM, -1))
				{
					Logger::LogError(lua_tostring(luaVM, -1));
				} else
				{
					Logger::LogError("No extra data\n");
				}
				is_script_running_ = false;
			} else if(run_result == LUA_ERRMEM)
			{//Memory allocation error, should report and abandon
				Logger::LogError("GameLua::Tick: A memory allocation error occurred while running\n");
				if(lua_isstring(luaVM, -1))
				{
					Logger::LogError(lua_tostring(luaVM, -1));
				}
				is_script_running_ = false;
			} else if(run_result == LUA_ERRERR)
			{ 
				Logger::LogError("GameLua::Tick: Error handler error\n");
				if(lua_isstring(luaVM, -1))
				{
					Logger::LogError(lua_tostring(luaVM, -1));
				}
				is_script_running_ = false;
			} else
			{
				int pt = lua_type(luaVM, -1);
				if((parameter_count == 2) && lua_isboolean(luaVM, -1)) //Stack should be Challenge.isDone
				{
					int isDone = lua_toboolean(luaVM, -1);
					if(isDone)
					{
						is_script_running_ = false;
						Logger::Log("Script over");
					}
				} else if((parameter_count == 2) && lua_isstring(luaVM, -1))
				{
					Logger::LogError("GameLua::Tick: EntryPoint returned an error\n");
					Logger::LogError(lua_tostring(luaVM, -1));
 					is_script_running_ = false;
				} else
				{
					Logger::LogError("GameLua::Tick: EntryPoint should return true or false, or an error string\n");
					is_script_running_ = false;
				}
				lua_pop(luaVM, 1); //Pop the boolean isDone
			}
		}
		else
		{
			Logger::LogError("GameLua::Tick: Unable to find EntryPoint function\n");
			is_script_running_ = false;
		}

		//ParseChallenge();
		lua_pop(luaVM, 1); //Pop the Challenge
	}
	else
	{
		Logger::LogError("GameLua::Tick: Unable to find Challenge table\n");
		lua_pop(luaVM, 1); //Pop what is probably nil
		is_script_running_ = false;
	}
}


void BaseLua::OverrideAI(BaseAI* _AI)
{
	if(core_)
	{
		core_->OverrideAI(_AI);
	}
	else
	{
		luaL_error(luaVM, "GameLua::Unable to override AI, no ship loaded");
		//TODO report errors
	}
}

void BaseLua::SetForceColor(int force, GLColor _color)
{
	force_colors_[force] = _color;
}

void BaseLua::SetFiringDelay(float _firing_delay)
{
	core_->SetFiringDelay(_firing_delay);
}

void BaseLua::ScaleHealth(float _scale)
{
	if(core_)
	{
		core_->ScaleHealth(_scale);
	}
	else
	{
		luaL_error(luaVM, "GameLua::Unable to scale health, no ship loaded");
	}
}

bool BaseLua::IsAlive(int _section_id)
{
	return game_->IsSectionAlive(_section_id);
}

BaseAI* BaseLua::GetAI()
{
	BaseAI* ai = NULL;
	lua_pushstring(luaVM, "AI");
	lua_gettable(luaVM, -2); //Pop AI and put AI table at top of stack
	if(lua_istable(luaVM, -1))
	{
		lua_pushstring(luaVM, "AIType");
		lua_gettable(luaVM, -2); //Pop AIType and puts the ai type on
		if(lua_isstring(luaVM, -1))
		{
			std::string ai_type_string = lua_tostring(luaVM, -1);
			std::transform(ai_type_string.begin(), ai_type_string.end(), ai_type_string.begin(), toupper);
			SectionType ai_type = SectionMap[ai_type_string];
			lua_pop(luaVM, 1); //Pop the AI type
			switch(ai_type)
			{
			case ai_RotatingAI:
				lua_pushstring(luaVM, "AIRotationRate");
				lua_gettable(luaVM, -2); //Puts rotation rate at top of stack
				if(lua_isnumber(luaVM, -1))
				{
					ai = new RotatingAI(static_cast<float>(lua_tonumber(luaVM, -1)));
				}
				else
				{
					ai = new RotatingAI(0.2f); //Default to 20% rotation rate
				}
				lua_pop(luaVM, 1); //Pop the AI rotation rate
				break;
			case ai_KeyboardAI:
				ai = new KeyboardAI();
				break;
			case ai_SimpleAI:
				ai = new SimpleAI();
				break;
			default:
				luaL_error(luaVM, "Unrecognised AIType");
				break;
			}

		}
		else
		{
			//Report AIType must be string
			luaL_error(luaVM, "AI table require a string AIType property");
			lua_pop(luaVM, 1);
		}
		lua_pop(luaVM, 1); //Pop the AI table
	} else
	{
		luaL_error(luaVM, "Cores require an AI table property");
		lua_pop(luaVM, 1); //Probably have nil at top of stack, so pop
	}
	if(ai == NULL)
	{
		ai = new RotatingAI(0.2f);
	}
	return ai;
}



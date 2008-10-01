#include "StdAfx.h"
#include "LuaAI.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static bool initialised_lua = false;

static int l_SetMoveDirection(lua_State* luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(luaVM, -3)));
	assert(instance);
	float x = static_cast<float>(lua_tonumber(luaVM, -2));
	float y = static_cast<float>(lua_tonumber(luaVM, -1));
	instance->SetMoveDirection(x, y);
	return 0;
}

void LuaAI::SetMoveDirection(float _x, float _y)
{
	Vector3f v = Vector3f(_x, _y, 0);
	if(v.lengthSq() > 1)
		v.normalize();
	next_move_.dx_ = v.x;
	next_move_.dy_ = v.y;
}

LuaAI::LuaAI(std::string _file_name, lua_State* _luaVM)
{
	if(!initialised_lua)
	{
		RegisterLuaFunctions(_luaVM);
		initialised_lua = true;
	}

	script_name_= _file_name;
	lua_state_ = _luaVM;
	next_move_ = AIAction();
	coroutine_reference_ = 0;
	environment_reference_ = 0;
	initialise_coroutine();
}

LuaAI::~LuaAI(void)
{
	//Should remove reference so GC can remove coroutine
	if(coroutine_reference_)
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, coroutine_reference_);
	if(environment_reference_)
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, environment_reference_);
}

bool LuaAI::initialise_coroutine()
{
	/* If already registered then remove it */
	if(coroutine_reference_)
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, coroutine_reference_);
	if(environment_reference_)
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, environment_reference_);

	/* Sandbox a file*/
	int load_error = luaL_loadfile(lua_state_, "AI_sandbox.lua");
	//Either have error message or chunk on stack
	if(load_error != 0 )
	{
		Logger::Instance() << lua_tostring(lua_state_, -1);
		return false;
	}
	lua_pushstring(lua_state_, script_name_.c_str());
	lua_pushlightuserdata(lua_state_, this);
	int run_error = lua_pcall(lua_state_,2, 2, NULL);
	if(run_error != 0 )
	{
		Logger::Instance() << lua_tostring(lua_state_, -1);
		return false;
	}
	//Should now have a reference to a coroutine and environment on stack
	environment_reference_ = luaL_ref(lua_state_, LUA_REGISTRYINDEX);
	coroutine_reference_ = luaL_ref(lua_state_, LUA_REGISTRYINDEX);

	return true;
}

void LuaAI::resume_coroutine(Core_ptr _self)
{
	if(coroutine_reference_)
	{
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, coroutine_reference_);
		lua_State* thread = lua_tothread(lua_state_, -1);
		lua_pop(lua_state_, 1);

		
		//Obtain ship object and update position etc
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, environment_reference_);
		//Environment on stack
		lua_pushstring(lua_state_, "ship");
		lua_gettable(lua_state_, -2);
		lua_pushstring(lua_state_, "position");
		lua_gettable(lua_state_, -2);
		lua_pushstring(lua_state_,"x");
		lua_pushnumber(lua_state_, _self->GetPosition().x);
		lua_settable(lua_state_, -3);
		lua_pushstring(lua_state_,"y");
		lua_pushnumber(lua_state_, _self->GetPosition().y);
		lua_settable(lua_state_, -3);
		lua_pop(lua_state_, 3);

		int resume_result = lua_resume(thread, 0);
		if(resume_result == LUA_YIELD)
		{
			//Logger::Instance() << "Coroutine yielded OK\n";
		} else if(resume_result == 0)
		{
			//Logger::Instance() << "Coroutine finished without errors\n";
			initialise_coroutine(); //Restart coroutine
		} else
		{
			Logger::Instance() << lua_tostring(thread, -1) << "\n";
		}
	}
}

AIAction LuaAI::Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self)
{
	next_move_ = AIAction(); /* Clears the next move, the resumes the AI		*/
	resume_coroutine(_self);		 /* which will call methods on the LuaAI, setting	*/
	return next_move_;		 /* the next_move_									*/
}

void LuaAI::RegisterLuaFunctions(lua_State* _luaVM)
{
	lua_register(_luaVM, "SetMoveDirection", l_SetMoveDirection);
}

LuaAI* LuaAI::FromScript(std::string _file_name, lua_State *_luaVM)
{
	int load_result = luaL_loadfile(_luaVM, _file_name.c_str());
	if(load_result==0)
	{
		lua_pop(_luaVM, 1); //Pop the chunk
		Logger::Instance() << "AI loaded OK\n";
		return new LuaAI(_file_name, _luaVM);
	} else
	{
		std::string error_string = lua_tostring(_luaVM, -1);
		lua_pop(_luaVM, 1);
		Logger::Instance() << "Load AI script error: " << error_string << "\n";
		return NULL;
	}
}
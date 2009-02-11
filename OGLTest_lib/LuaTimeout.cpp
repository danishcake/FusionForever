#include "stdafx.h"
#include "LuaTimeout.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include <sdl.h>

LuaTimeout::LuaTimeout(std::string _message, float _timeout)
{
	abort_message = _message;
	call_id = 0;
	active = false;
	luaVM = NULL;
	timeout_ = _timeout;
	thread = SDL_CreateThread(ExecutionMonitor, this);
}

LuaTimeout::~LuaTimeout()
{
	SDL_KillThread(thread);
}

int LuaTimeout::SafeResume(lua_State* coroutine)
{
	luaVM = coroutine;
	
	call_id++;
	active = true;
	
	int resume_result = lua_resume(coroutine, 0);
	active = false;
	if(luaL_getbailout(luaVM))
	{
		lua_pop(luaVM, lua_gettop(luaVM)); //Stack may well be trashed, so clear it
		lua_pushstring(coroutine, abort_message.c_str());
		return LUA_ERRRUN;
	}
	return resume_result;
}

/*
 * Even in race conditions the worst that will happen is that the error is picked up on the next resume.
 */
int LuaTimeout::ExecutionMonitor(void* param)
{
	LuaTimeout* td = static_cast<LuaTimeout*>(param);
	int call_id = 0;
	float watch_time = 0;
	while(true)
	{
		SDL_Delay(100);
		//New script to monitor, usually indicates that all is well
		if(td->call_id != call_id)
		{
			watch_time = 0;
			call_id = td->call_id;
		}
		if(td->active && td->luaVM != NULL)
		{
			watch_time += 0.1f;
			if(watch_time > td->timeout_)
			{
				// This function requires a patch to be applied to lua
				luaL_setbailout(td->luaVM, 1);
			}
		}
	}
}
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
	if(resume_result != LUA_YIELD && resume_result != 0)
	{
		Logger::ErrorOut() << "Lua Error: Begin stack dump\n";
		//Stack is not unwound, so can dump it
		
		/* Stack dump lifted wholesale from http://www.lua.org/pil/24.2.3.html */
		int i;
		int top = lua_gettop(coroutine);
		for (i = 1; i <= top; i++) {  /* repeat for each level */
			int t = lua_type(coroutine, i);
			switch (t) {
				case LUA_TSTRING:  /* strings */
					Logger::ErrorOut() << i << " String: " << lua_tostring(coroutine, i);
				break;

				case LUA_TBOOLEAN:  /* booleans */
					Logger::ErrorOut() << i << " Boolean: " << (lua_toboolean(coroutine, i) ? "true" : "false");
				break;

				case LUA_TNUMBER:  /* numbers */
					Logger::ErrorOut() << i << " Number: " << lua_tonumber(coroutine, i);
				break;

				default:  /* other values */
					Logger::ErrorOut() << i << " Other: " << lua_typename(coroutine, t);
				break;
			}
			Logger::ErrorOut() << "\n";
		}
		Logger::ErrorOut() << "\n End stack dump\n"; /* end the listing */
	
		//Now tidy up stack. Error message should be left at the top, but everything else cleared away
		lua_insert(coroutine, 1);
		lua_pop(coroutine, lua_gettop(coroutine) - 1);
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
	for(;;)
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
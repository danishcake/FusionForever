#include "StdAfx.h"
#include "SandboxCache.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

SandboxCache::SandboxCache(lua_State* _luaVM)
{
	luaVM_ = _luaVM;
	sandbox_reference_ = LUA_NOREF;
}

SandboxCache::~SandboxCache(void)
{
}

int SandboxCache::GetReference(std::string _id)
{
	if(chunk_reference_.find(_id) != chunk_reference_.end())
	{
		return chunk_reference_[_id];
	} else
		return LUA_NOREF;
}

int SandboxCache::AddReference(std::string _id)
{
	int reference = luaL_ref(luaVM_, LUA_REGISTRYINDEX);
	chunk_reference_[_id] = reference;
	return reference;
}

int SandboxCache::SetSandboxReference()
{
	sandbox_reference_ = luaL_ref(luaVM_, LUA_REGISTRYINDEX);
	return sandbox_reference_;
}

int SandboxCache::GetSandboxReference()
{
	return sandbox_reference_;
}

lua_State* SandboxCache::GetLuaVM()
{
	return luaVM_;
}
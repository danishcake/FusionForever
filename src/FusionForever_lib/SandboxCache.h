#pragma once
#include <map>
#include <string>

struct lua_State;

/* Caches reusable scripts - designed to stores state for the luaAI, but could 
   also be used for scripted sections if I ever make them */
class SandboxCache
{
private:
	std::map<std::string, int> chunk_reference_;
	int sandbox_reference_;
	lua_State* luaVM_;
public:
	SandboxCache(lua_State* _luaVM);
	~SandboxCache(void);

	int GetReference(std::string _id);
	/*
	 * Stores the top of the lua stack in the registery and pops stack
	 * Returns index of item in registry
	 */
	int AddReference(std::string _id);

	/*
	 * Stores the top of the lua stack in the registery and pops stack
	 */
	int SetSandboxReference();
	int GetSandboxReference();

	lua_State* GetLuaVM();
};

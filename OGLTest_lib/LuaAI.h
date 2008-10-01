#pragma once
#include <string>
#include "Core.h"

struct lua_State;

class LuaAI :
	public BaseAI
{
protected:
	std::string script_name_;
	lua_State* lua_state_;
	int coroutine_reference_;
	int environment_reference_;
	AIAction next_move_;

	bool initialise_coroutine();
	void resume_coroutine(Core_ptr _self);

public:
	LuaAI(std::string _file_name, lua_State* _luaVM);
	~LuaAI(void);
	

	virtual AIAction Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self);
	
	void SetMoveDirection(float _x, float _y);

	static void RegisterLuaFunctions(lua_State* _luaVM);
	static LuaAI* FromScript(std::string _file_name, lua_State* _luaVM);
};
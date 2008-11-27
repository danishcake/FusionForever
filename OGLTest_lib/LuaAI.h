#pragma once
#include <string>
#include "BaseAI.h"

struct lua_State;

class LuaAI :
	public BaseAI
{
protected:
	std::string script_name_;
	lua_State* lua_state_;
	int coroutine_reference_;
	int environment_reference_;
	int chunk_reference_;
	AIAction next_move_;

	bool initialise_coroutine();
	void resume_coroutine(Core* _self, float _timespan);

	Core* target_;
	Core* self_;

	bool pick_random_next;
	bool pick_closest_next;

	float sum_time_;

	/*
	 * True while no error has been encountered. Once an error is found it is set false and the AI grinds to a halt
	 */
	bool ok_to_run_;

public:
	LuaAI(std::string _file_name, int _chunk_reference, lua_State* _luaVM);
	~LuaAI(void);

	virtual AIAction Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self);
	
	void SetMoveDirection(float _x, float _y);
	void SetTurnDirection(float _dtheta);
	void SetAll(float _x, float _y, float _dtheta, bool _firing);

	void PickRandomTarget();
	void PickClosestTarget();

	void SetCameraPosition(float _x, float _y);

	static void RegisterLuaFunctions(lua_State* _luaVM);
	static LuaAI* FromScript(std::string _file_name, lua_State* _luaVM);

	virtual void EndSubscription(Subscriber* _source);

	static bool initialised_lua;
};
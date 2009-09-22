#pragma once
#include <string>
#include "BaseAI.h"
#include <map>

struct lua_State;
class LuaTimeout;
class SandboxCache;

class LuaAI :
	public BaseAI
{
private:
	/* Monitor to abort long running scripts */
	static LuaTimeout* monitor_thread_;

	/* Lua functions to be called from scripts */
	static int l_SetMoveDirection(lua_State* _luaVM);
	static int l_SetTurnDirection(lua_State* _luaVM);
	static int l_SetAll(lua_State* _luaVM);
	static int l_PickRandomTarget(lua_State* _luaVM);
	static int l_PickClosestTarget(lua_State* _luaVM);
	static int l_SetCameraPosition(lua_State* _luaVM);
	static int l_ChangeAI(lua_State* _luaVM);

	/*
	 * Static initialisation. Called once per creation of a lua_State by a new LuaChallenge
	 */
	static void RegisterLuaFunctions(SandboxCache* _cache);

protected:
	LuaAI(std::string _file_name, int _chunk_reference, SandboxCache* _cache);
	/* Filename of the AI script */
	std::string script_name_;
	
	/* Global lua_State passed from BaseGame via LuaChallenge */
	lua_State* luaVM_;
	/* Cache of entries - kept so that the LuaAI can change AI types by itself */
	SandboxCache* cache_;
	/* Index of the sandbox */
	int ai_sandbox_reference_;
	/* Index of AI coroutine in registry */
	int coroutine_reference_;
	/* Index of environment table in registry */
	int environment_reference_;
	/* Index of compiled AI chunk in registry */
	int chunk_reference_;
	/* The next move to perform. Cleared at the start of Tick() and manipulated from lua */
	AIAction next_move_;

	/*
	 * Initialises the coroutine. Creates environment if none exists and stores in registry
	 * Creates a new coroutine and stores in registry.
	 */
	bool initialise_coroutine();

	/*
	 * Updates the AI environment, and resumes the coroutine
	 */
	void resume_coroutine(Core* _self, float _timespan);

	/*
	 * Pointers to self and target. Target uses subscriptions to ensure unsubscription on death
	 */
	Core* target_;
	Core* self_;

	/*
	 * Set high so that on next Tick a new target will be picked, then cleared down
	 */
	bool pick_random_next_;
	bool pick_closest_next_;

	/* The total running time of the AI. */
	float sum_time_;

	/*
	 * True while no error has been encountered. Once an error is found it is set false and the AI grinds to a halt
	 */
	bool ok_to_run_;

public:
	~LuaAI(void);

	/*
	 * Resumes/restarts the AI script and returns the result
	 */
	virtual AIAction Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self);
	
	/*
	 * Provided to allow LuaChallenge to force the AI to target a particular ship
	 */
	virtual void SpecifyTarget(Core* _target);

	/*
	 * Reloads the AI with a new AI, called from AI script
	 */
	void ChangeAI(std::string _filename);

	/*
	 * Clears down initialisation, so that on creation of next LuaAI the lua functions are reregistered.
	 * Called by BaseGame on initialisation
	 */
	//static void SetUninitialised();
	/*
	 * Returns a LuaAI created from the specified script file, or NULL in case of failure
	 */
	static LuaAI* FromScript(std::string _file_name, SandboxCache* _luaVM);

	virtual void EndSubscription(Subscriber* _source);

	
};
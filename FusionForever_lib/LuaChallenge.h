#pragma once
#include <stack>
#include <string>
#include "vmath.h"
#include "GLColor.h"
#include "SandboxCache.h"


struct lua_State;
class LuaTimeout;

#ifndef STR_ME
#define STR_ME( X ) ( # X )
#endif

namespace ChallengeState
{
	static enum Enum
	{
		NotStarted,
		Running,
		Victory,
		Defeat,
		Draw,
		ReturnToEditor,
		LoadError,
		RunError
	};

	static const std::string ToStr[] = {
		STR_ME( NotStarted ),
		STR_ME( Running ),
		STR_ME( Victory ),
		STR_ME( Defeat ),
		STR_ME( Draw ),
		STR_ME( ReturnToEditor ),
		STR_ME( LoadError ),
		STR_ME( RunError ),
		};
}

class BaseGame;
class Section;
class Core;

class LuaChallenge
{
private:
	static int l_SpawnShip(lua_State* _luaVM);
	static int l_KillShip(lua_State* _luaVM);
	static int l_GetShipsInArea(lua_State* _luaVM);
	static int l_SetDeathFunction(lua_State* _luaVM);
	static int l_luaError(lua_State* _luaVM);
	static int l_Victory(lua_State* _luaVM);
	static int l_Defeat(lua_State* _luaVM);
	static int l_Draw(lua_State* _luaVM);
	static int l_ReturnToEditor(lua_State* _luaVM);
	static int l_GetShipData(lua_State* _luaVM);
	static int l_SetShipTarget(lua_State* _luaVM);
	static int l_SetHostility(lua_State* _luaVM);
	static int l_DisplayMessage(lua_State* _luaVM);
	static int l_SetCounter(lua_State* _luaVM);

	
	static void ParseExistingShip(lua_State* _luaVM, Section* _section, int _child_key, int _stack_size);
	static int l_GetDesign(lua_State* _luaVM);
	static int l_SpawnDesign(lua_State* _luaVM);

	static void ParseLuaShip(lua_State* _luaVM, Section** _parent, bool _update_mode, LuaChallenge* _challenge, std::stack<std::vector<int>>& indices);
	static int l_UpdateDesign(lua_State* _luaVM);
	

	GLColor force_colors_[8];
	
	lua_State* luaVM_;
	BaseGame* game_;
	std::string challenge_;
	ChallengeState::Enum state_;
	float sum_time_;
	
	LuaTimeout* monitor_thread_;
	SandboxCache luaAIcache_;

	int chunk_reference_;
	int environment_reference_;
	int coroutine_reference_;
	
	bool loadChallenge();
	bool createEnvironmentAndCoroutine();

	//Non obvious reason for passing _luaVM - if lual_error is called from c function not called from lua then it exits. This means I need the lua_State passed by lua
	int SpawnShip(std::string _ship_name, int _force, Vector2f _position, float angle, std::string _ai_script, float _health_scale, lua_State* _luaVM);
	int SpawnDesign(Core* _core, int _force);
	void DeclareVictory() {state_ = ChallengeState::Victory;}
	void DeclareDefeat() {state_ = ChallengeState::Defeat;}
	void DeclareDraw() {state_ = ChallengeState::Draw;}
	void ReturnToEditor(){state_ = ChallengeState::ReturnToEditor;}
	void SetHostility(int _force_b, int force_b, bool _hostility);
	Core* GetShipData(int _ship_id);
	Section* GetSectionData(int _section_id);
	std::vector<Core*> GetShipsInArea(Vector3f _position, float _radius);
	void DisplayMessage(std::string _message, float _time);
	void SetCounter(int _counter, int _value, int _max, bool _visible);

public:
	LuaChallenge(lua_State* _luaVM, std::string _challenge, BaseGame* _game);
	~LuaChallenge(void);

	ChallengeState::Enum Tick(float _timespan);
	void CallDeathFunction(int _death_function_refence);
	SandboxCache* GetLuaAICache(){return &luaAIcache_;}
};

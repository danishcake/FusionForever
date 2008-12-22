#pragma once
#include <string>
#include "vmath.h"
#include "GLColor.h"

struct lua_State;

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
//class Section;
class Core;

class LuaChallenge
{
private:
	static int l_SpawnShip(lua_State* _luaVM);
	static int l_SetDeathFunction(lua_State* _luaVM);
	static int l_luaError(lua_State* _luaVM);
	static int l_Victory(lua_State* _luaVM);
	static int l_Defeat(lua_State* _luaVM);
	static int l_Draw(lua_State* _luaVM);
	static int l_ReturnToEditor(lua_State* _luaVM);
	static int l_GetShipData(lua_State* _luaVM);
	static int l_SetShipTarget(lua_State* _luaVM);
	
	GLColor force_colors_[8];
	
	lua_State* luaVM_;
	BaseGame* game_;
	std::string challenge_;
	ChallengeState::Enum state_;
	float sum_time_;

	int chunk_reference_;
	int environment_reference_;
	int coroutine_reference_;
	
	bool loadChallenge();
	bool createEnvironmentAndCoroutine();

	//Non obvious reason for passing _luaVM - if lual_error is called from c function not called from lua then it exits. This means I need the lua_State passed by lua
	int SpawnShip(std::string _ship_name, int _force, Vector2f _position, float angle, std::string _ai_script, float _health_scale, lua_State* _luaVM);
	void DeclareVictory() {state_ = ChallengeState::Victory;}
	void DeclareDefeat() {state_ = ChallengeState::Defeat;}
	void DeclareDraw() {state_ = ChallengeState::Draw;}
	void ReturnToEditor(){state_ = ChallengeState::ReturnToEditor;}
	Core* GetShipData(int _ship_id);

public:
	LuaChallenge(lua_State* _luaVM, std::string _challenge, BaseGame* _game);
	~LuaChallenge(void);

	ChallengeState::Enum Tick(float _timespan);
	void CallDeathFunction(int _death_function_refence);
};

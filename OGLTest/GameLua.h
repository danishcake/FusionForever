#pragma once
#include <stack>
#include <list>
#include "Section.h"
#include "Core.h" //For BaseAI define therein

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class GameLua
{
private:
	std::list<Core_ptr> enemies_;
	std::list<Core_ptr> friends_;
	std::stack<Section*> section_stack_;
	BaseAI* GetAI();
	bool is_script_running_;
public:
	GameLua(void);
	~GameLua(void);
	
	void LoadChallenge(const char* challenge);
	void LoadShip(const char* ship);
	void ParseShip();

	void PushCore(Core* _core);
	void PushSection(Section* _section);
	void PopSection();

	void SetAngle(float _angle);
	void SetPosition(float _x, float _y);
	void SetHealth(float _health);

	void AddAsFriend();
	void AddAsEnemy();

	void Tick();
	lua_State* GetLuaVM();

	std::list<Core_ptr>& GetEnemies();
	std::list<Core_ptr>& GetFriends();
};

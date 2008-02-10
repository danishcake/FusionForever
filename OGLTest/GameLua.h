#pragma once
#include <stack>
#include <list>
#include "Section.h"
#include "Core.h" //For BaseAI define therein
#include <lua.h>

class GameLua
{
private:
	std::list<Core_ptr> enemies_;
	std::list<Core_ptr> friends_;
	std::stack<Section*> section_stack_;
	BaseAI* GetAI();
public:
	GameLua(void);
	~GameLua(void);
	
	void PushCore(Core* _core);
	void PushSection(Section* _section);
	void PopSection();
	void AddAsFriend();
	void AddAsEnemy();
	lua_State* GetLuaVM();

	std::list<Core_ptr>& GetEnemies();
	std::list<Core_ptr>& GetFriends();
};

#pragma once
#include <stack>
#include <list>
#include "Section.h"
#include "Core.h" //For BaseAI define therein
#include "GLColor.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class GameScene;

class GameLua
{
private:
	std::list<Core_ptr> enemies_;
	std::list<Core_ptr> friends_;
	std::stack<Section_ptr> section_stack_;
	BaseAI* GetAI();
	bool is_script_running_;
	float sum_time_;
	GameScene* game_scene_;
public:
	GameLua(GameScene* _game_scene);
	~GameLua(void);
	

	void LoadChallenge(const char* challenge);
	int LoadShip(const char* _ship);
	void ParseShip(const char* _ship);

	void PushCore(Core_ptr _core);
	void PushSection(Section_ptr _section);
	void PopSection();

	void SetAngle(float _angle);
	void SetPosition(float _x, float _y);
	void SetHealth(float _health);
	void ScaleHealth(float _scale);
	void SetColor(GLColor _color);
	void SetFiringDelay(float _firing_delay);

	void AddAsFriend();
	void AddAsEnemy();

	bool IsAlive(int _section_id);

	void StackToCore();
	void OverrideAI(BaseAI*);

	void Tick(int _friend_count, int _enemy_count, float _timespan);
	lua_State* GetLuaVM();

	std::list<Core_ptr>& GetEnemies();
	std::list<Core_ptr>& GetFriends();
};

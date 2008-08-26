#pragma once
#include <vector>
#include "Core.h"
#include "Starfield.h"
#include "GridCollisionManager.h"
#include <string>

static const int MAX_FORCES = 8;

namespace Hostility
{
	typedef enum Enum
	{
		Friendly,
		Hostile
	};
}

class BaseLua;
struct lua_State;

class BaseGame
{
public:
	BaseGame(void);
	virtual ~BaseGame(void);

	virtual void Tick(float _timespan);
	virtual void Draw();
	void LoadChallenge(std::string _challenge);
	void AddShip(Core* _core, int _force);
	int GetForceCount(int _force);
	int GetEnemyCount(int _force);
	int GetFriendCount(int _force);
	bool IsSectionAlive(int _section_id);
	lua_State* GetLua();

protected:
	std::vector<Projectile_ptr> projectiles_[MAX_FORCES];
	std::vector<Decoration_ptr> decorations_;
	std::vector<Core_ptr> ships_[MAX_FORCES];
	GridCollisionManager collision_managers_[MAX_FORCES];
	Hostility::Enum hostility_[MAX_FORCES][MAX_FORCES];
	std::vector<Core_ptr> enemies[MAX_FORCES];
	std::vector<Core_ptr> friends[MAX_FORCES];

	Starfield starfield_;
	BaseLua* lua_;
};

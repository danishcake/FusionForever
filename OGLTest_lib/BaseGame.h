#pragma once
#include <vector>
#include "Core.h"
#include "Starfield.h"
#include "GridCollisionManager.h"
#include <string>
#include "Radar.h"
#include "GameGui.h"

static const int MAX_FORCES = 8;

namespace Hostility
{
	typedef enum Enum
	{
		Friendly,
		Hostile
	};
}

struct lua_State;
class LuaChallenge;

class BaseGame
{
public:
	BaseGame(std::string _challenge_filename);
	virtual ~BaseGame(void);

	virtual int Tick(float _timespan, GameGUI& _gui);
	virtual void Draw();
	void LoadChallenge(std::string _challenge);
	void AddShip(Core* _core, int _force);
	void SetHostility(int _force_a, int _force_b, bool _hostile);

	int GetForceCount(int _force);
	int GetEnemyCount(int _force);
	int GetFriendCount(int _force);
	Section* GetSectionData(int _section_id);

	void DisplayMessage(std::string _message, float _time);
	void SetCounter(int _counter_id, int _value, int _max, bool _visible);

protected:
	std::vector<Projectile_ptr> projectiles_[MAX_FORCES];
	std::vector<Decoration_ptr> decorations_;
	std::vector<Core_ptr> ships_[MAX_FORCES];
	GridCollisionManager collision_managers_[MAX_FORCES];
	Hostility::Enum hostility_[MAX_FORCES][MAX_FORCES];
	std::vector<Core_ptr> enemies[MAX_FORCES];
	std::vector<Core_ptr> friends[MAX_FORCES];

	Starfield starfield_;
	Radar radar_;
	LuaChallenge* challenge_;
	lua_State* luaVM_;

	GLuint victory_texture_;
	GLuint defeat_texture_;

	int player_id_; //The id of the first player controlled by a keyboard
	GameGUI gui_; //Contains counter data
};

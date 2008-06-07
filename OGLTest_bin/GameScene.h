#pragma once
#include "BaseScene.h"
#include <vector>
#include <list>
#include "Core.h"
#include "Starfield.h"
#include "GameLua.h"

class GameScene :
	public BaseScene
{
public:
	GameScene(void);

	virtual ~GameScene(void);
	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();
	bool IsSectionAlive(int _section_id);

protected:
	std::vector<Projectile_ptr> enemy_projectiles;
	std::vector<Projectile_ptr> ownship_projectiles;
	std::vector<Decoration_ptr> decorations;

	std::vector<Core_ptr> friends_;
	std::vector<Core_ptr> enemies_;

	ICollisionManager* enemies_sp_;
	ICollisionManager* friends_sp_;
	Starfield starfield_;
	GameLua* game_lua_;
};

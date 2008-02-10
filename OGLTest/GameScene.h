#pragma once
#include "BaseScene.h"
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

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
	
	void AddEnemy(Core* _enemy);

protected:
	std::list<Projectile_ptr> enemy_projectiles;
	std::list<Projectile_ptr> ownship_projectiles;
	std::list<Decoration_ptr> decorations;

	std::list<Core_ptr> friends_;
	std::list<Core_ptr> enemies_;
	Starfield starfield_;
	GameLua game_lua_;

	virtual void initSections();
};

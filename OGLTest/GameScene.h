#pragma once
#include "BaseScene.h"
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "Core.h"
#include "Starfield.h"

typedef boost::shared_ptr<Projectile> Projectile_ptr;
typedef boost::shared_ptr<Decoration> Decoration_ptr;
typedef boost::shared_ptr<Core> Core_ptr;


class GameScene :
	public BaseScene
{
public:
	GameScene(void);

	virtual ~GameScene(void);
	virtual void Tick(float _timespan, std::vector<boost::shared_ptr<BaseScene>>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();

protected:
	std::list<Projectile_ptr> enemy_projectiles;
	std::list<Projectile_ptr> ownship_projectiles;
	std::list<Decoration_ptr> decorations;

	std::list<Core_ptr> friends_;
	std::list<Core_ptr> enemies_;
	Core_ptr ownship_;
	Starfield starfield_;
	virtual void initSections();
};

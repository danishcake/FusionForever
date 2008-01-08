#pragma once
#include "basescene.h"

class GameScene :
	public BaseScene
{
public:
	GameScene(void);
public:
	virtual ~GameScene(void);
	virtual void Tick(float _timespan, std::vector<boost::shared_ptr<BaseScene>>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();
};

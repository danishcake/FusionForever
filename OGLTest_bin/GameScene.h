#pragma once
#include "BaseScene.h"
#include "Core.h"
#include "Starfield.h"
#include "BaseGame.h"

class GameScene :
	public BaseScene
{
public:
	GameScene(std::string _challenge);

	virtual ~GameScene();
	virtual void Tick(float _timespan, std::vector<BaseScene_ptr>& _new_scenes);
	virtual void Draw();
	virtual bool IsRoot();
	virtual bool IsRemovable();
	bool IsSectionAlive(int _section_id);
protected:
	BaseGame* game_;

	bool returning_to_menu_;
	bool spawn_fadeout_;
	float timer_;
};

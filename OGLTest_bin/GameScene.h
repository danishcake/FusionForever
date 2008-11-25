#pragma once
#include "BaseScene.h"
#include "Core.h"
#include "Starfield.h"
#include "BaseGame.h"
#include "Billboard.h"

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
	boost::shared_ptr<Billboard> end_billboard_;
	bool returning_to_menu_;
	bool spawn_fadeout_;
	float timer_;

	std::string challenge_;
};

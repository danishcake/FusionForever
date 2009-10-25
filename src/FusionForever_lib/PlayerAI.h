#pragma once
#include "BaseAI.h"

class PlayerAI :
	public BaseAI
{
protected:
	int player_id_;
	Vector3f ltv_mouse_position_;
	Vector3f movement_integrator_;
public:
	PlayerAI(int _player_id);
	virtual ~PlayerAI(void);
	virtual AIAction Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self);
	virtual void SpecifyTarget(Core*){}
	virtual bool IsHuman(){return true;}

	static void LoadBindings();
};

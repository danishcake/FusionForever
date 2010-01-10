#pragma once
#include "BaseAI.h"
#include "vmath.h"
#include "Binding.h"

class PlayerAI :
	public BaseAI
{
protected:
	int player_id_;
	int core_id_;
	static std::vector<int> core_ids_;

	Vector3f ltv_mouse_position_;
	Vector3f movement_integrator_;
	bool lock_angle_;
	Vector3f lock_vector_;

	static std::map<int, std::vector<InputConfig> > bindings;

public:
	PlayerAI(int _player_id);
	virtual ~PlayerAI(void);
	virtual AIAction Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self);
	virtual void SpecifyTarget(Core*){}
	virtual bool IsHuman(){return true;}

	static void LoadBindings();
	static void SaveBindings();
	static void ResetToDefault();
	static std::vector<int> GetPlayerIDs(){return core_ids_;}
	static std::map<int, std::vector<InputConfig> >&  GetBindings(){return bindings;}
};

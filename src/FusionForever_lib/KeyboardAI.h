#pragma once
#include "BaseAI.h"
#include "sdl.h"

class KeyboardAI :
	public BaseAI
{
protected:
	static int instance_count_;
	static std::vector<int> player_ids_;
	int player_id_;
	static float zoom_factor_;
public:
	KeyboardAI(void);
	virtual ~KeyboardAI(void);
	virtual AIAction Tick(float _timespan, std::vector<Core*>& _allies, std::vector<Core*>& _enemies, Core* _self);
	virtual void SpecifyTarget(Core*){}
	virtual bool IsHuman(){return true;}

	static std::vector<int> GetPlayerIDs(){return player_ids_;}
};

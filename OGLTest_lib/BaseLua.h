#pragma once
#include "Section.h"
#include "Core.h" //For BaseAI defined therein
#include "GLColor.h"
#include "BaseGame.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class BaseLua
{
private:
	BaseAI* GetAI();

	std::stack<Section_ptr> section_stack_;
	BaseGame* game_;
	bool is_script_running_;
	float sum_time_;
	GLColor force_colors_[MAX_FORCES];

protected:
	void StackToCore();

public:
	BaseLua(BaseGame* _game);
	~BaseLua(void);

	void LoadChallenge(const char* challenge);
	int LoadShip(const char* _ship);
	void ParseShip(const char* _ship);

	void PushCore(Core_ptr _core);
	void PushSection(Section_ptr _section);
	void PopSection();

	void SetAngle(float _angle);
	void SetPosition(float _x, float _y);
	void SetHealth(float _health);
	void ScaleHealth(float _scale);
	void SetForceColor(int _force, GLColor _color);
	void SetFiringDelay(float _firing_delay);

	void AddToForce(int _force_id);
	bool IsAlive(int _section_id);
	void OverrideAI(BaseAI*);

	void Tick(float _timespan);
};

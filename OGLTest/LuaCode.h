#pragma once
#include <stack>
#include "Section.h"
#include "Core.h" //For BaseAI define therein

enum AIType
{
	AI_Rotating,
	AI_Keyboard 
};

class LuaCode
{
private:
	std::stack<Section_ptr> section_stack_;
	BaseAI* GetAI();
	AIType ai_type_;	
	float ai_rot_rate_;

public:
	LuaCode(void);
	~LuaCode(void);
	

	void PushSquareCore(void);
	void SetAIRotating(float _rot_rate);
};

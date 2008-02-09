#pragma once
#include <stack>
#include <list>
#include "Section.h"
#include "Core.h" //For BaseAI define therein

class LuaCode
{
private:
	std::list<Core_ptr> enemies_;
	std::list<Core_ptr> friends_;
	std::stack<Section*> section_stack_;
	BaseAI* GetAI();
public:
	LuaCode(void);
	~LuaCode(void);
	
	void PushCore(Core* _core);
	void PushSection(Section* _section);
	void PopSection();
	void AddAsFriend();
	void AddAsEnemy();


};

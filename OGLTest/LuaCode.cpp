#include "StdAfx.h"
#include "LuaCode.h"

#include "SquareCore.h"
#include "RigidArm.h"

#include "RotatingAI.h"
#include "KeyboardAI.h"

#include <string>
#include <map>
#include <algorithm>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static LuaCode* last_instantiation = NULL;
static lua_State* luaVM;

static enum SectionType
{
	st_SquareCore, 
	st_RigidArm
};
static std::map<std::string, SectionType> SectionMap;
static void InitialiseMap()
{
	SectionMap["SQUARECORE"] = st_SquareCore;
	SectionMap["RIGIDARM"] = st_RigidArm;
}

static int l_push_section(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);

	std::string section_string = lua_tostring(luaVM, 1);
	std::transform(section_string.begin(),section_string.end(),section_string.begin(),toupper);
	SectionType s_type = SectionMap[section_string];
	switch(s_type)
	{
		BaseAI* ai;
		case st_SquareCore:
			{
				std::string ai_string = lua_tostring(luaVM, 2);
				std::transform(ai_string.begin(), ai_string.end(), ai_string.begin(), toupper);
				
				if(ai_string.compare("KEYBOARDAI"))
				{
				} else if(ai_string.compare("ROTATINGAI"))
				{
					float rotRate = (float)lua_tonumber(luaVM,3);
					ai = new RotatingAI(rotRate);
				} else //Default to rotating
				{
					ai = new RotatingAI(0.2f);
				}
			}
			last_instantiation->PushCore(new SquareCore(ai));
		return 0;
		break;
		case st_RigidArm:
			last_instantiation->PushSection(new RigidArm());
			return 0;
		break;
		default:
			return -1; //Not handled
			break;
	}
}

static int l_pop(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);

	last_instantiation->PopSection();
	return 0;
}

static int l_add_as_enemy(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);

	last_instantiation->AddAsEnemy();
	return 0;
}

static int l_add_as_friend(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);

	last_instantiation->AddAsFriend();
	return 0;
}

LuaCode::LuaCode(void)
{
	if(last_instantiation == NULL)
	{
		InitialiseMap(); //Allows me to use a big switch statement
		luaVM = lua_open();
		//Register all the lua functions
		lua_register(luaVM, "PushSection", l_push_section);
		lua_register(luaVM, "PopSection", l_pop);
		lua_register(luaVM, "AddAsEnemy", l_add_as_enemy);
		lua_register(luaVM, "AddAsFriend", l_add_as_friend);
	}
	last_instantiation = this;
}

LuaCode::~LuaCode(void)
{
	while(!section_stack_.empty())
	{
		if(section_stack_.size() == 1)
		{
			delete section_stack_.top();
		}
		section_stack_.pop();
	}
}

void LuaCode::PushCore(Core* _core)
{
	while(!section_stack_.empty())
	{
		if(section_stack_.size() == 1)
		{
			delete section_stack_.top();
		}
		section_stack_.pop();
	}
	section_stack_.push(_core);
}

void LuaCode::PushSection(Section* _section)
{
	if(!section_stack_.empty())
	{		
		section_stack_.top()->AddChild(_section);
		section_stack_.push(_section);		
	}
}

void LuaCode::PopSection()
{
	if(section_stack_.size() > 1)
	{
		section_stack_.pop();
	}
}

void LuaCode::AddAsFriend()
{
	if(!section_stack_.empty())
	{
		while(section_stack_.size() > 1)
		{
			section_stack_.pop();
		}
		friends_.push_back(Core_ptr((Core*)section_stack_.top()));
		section_stack_.pop();
	}
}

void LuaCode::AddAsEnemy()
{
	if(!section_stack_.empty())
	{
		while(section_stack_.size() > 1)
		{
			section_stack_.pop();
		}
		enemies_.push_back(Core_ptr((Core*)section_stack_.top()));
		section_stack_.pop();
	}
}

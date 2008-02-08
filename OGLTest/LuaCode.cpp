#include "StdAfx.h"
#include "LuaCode.h"

#include "SquareCore.h"

#include "RotatingAI.h"
#include "KeyboardAI.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static LuaCode* last_instantiation = NULL;
static lua_State* luaVM;

//Lua callbacks
static int l_PushSquareCore(lua_State* luaVM)
{
	last_instantiation->PushSquareCore();
	return 0;
}

static int l_SetAI_Rotating(lua_State* luaVM)
{
	last_instantiation->SetAIRotating((float)lua_tonumber(luaVM, 1));
}

void LuaCode::SetAIRotating(float _rot_rate)
{
	ai_type_ = AI_Rotating;
	ai_rot_rate_ = _rot_rate;	
}

void LuaCode::PushSquareCore()
{
	//Cannot add a core if not empty
	if(section_stack_.empty())
	{
		section_stack_.push(Section_ptr(new SquareCore(GetAI())));
	}
	else
	{
		//Should raise some sort of error here.
	}
}

LuaCode::LuaCode(void)
{
	if(last_instantiation == NULL)
	{
		luaVM = lua_open();
		//Register all the lua functions
		lua_register(luaVM, "PushSquareCore", l_PushSquareCore);
	}
	last_instantiation = this;
	ai_type_ = AI_Rotating;
}

LuaCode::~LuaCode(void)
{
}

BaseAI* LuaCode::GetAI()
{
	switch(ai_type_)
	{
	case AI_Rotating:
		return new RotatingAI(1);
		break;
	case AI_Keyboard:
		return new KeyboardAI();
		break;
	default:
		return new RotatingAI(1);
		break;
	}
}
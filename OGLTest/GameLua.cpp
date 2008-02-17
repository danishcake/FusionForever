#include "StdAfx.h"
#include "GameLua.h"

#include "SquareCore.h"
#include "RigidArm.h"
#include "Blaster.h"
#include "HeatBeamGun.h"
#include "SpinningJoint.h"

#include "RotatingAI.h"
#include "KeyboardAI.h"

#include <string>
#include <map>
#include <algorithm>

static GameLua* last_instantiation = NULL;
lua_State* luaVM;

static enum SectionType
{
	unknown_key,
	st_SquareCore, 
	st_RigidArm,
	st_Blaster,
	st_HeatBeam,
	st_SpinningJoint,
	ai_RotatingAI,
	ai_KeyboardAI
};
static std::map<std::string, SectionType> SectionMap;
static void InitialiseMap()
{
	SectionMap["SQUARECORE"] = st_SquareCore;
	SectionMap["RIGIDARM"] = st_RigidArm;
	SectionMap["KEYBOARDAI"] = ai_KeyboardAI;
	SectionMap["ROTATINGAI"] = ai_RotatingAI;
	SectionMap["BLASTER"] = st_Blaster;
	SectionMap["HEATBEAM"] = st_HeatBeam;
	SectionMap["SPINNINGJOINT"] = st_SpinningJoint;
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
				
				SectionType ai_type = SectionMap[ai_string];
				switch(ai_type)
				{
				case ai_RotatingAI:
					{
					float rotRate = (float)lua_tonumber(luaVM, 3);
					ai = new RotatingAI(rotRate);
					}
					break;
				case ai_KeyboardAI:
					ai = new KeyboardAI();
					break;
				default:
					ai = new RotatingAI(0.2f);
					break;
				}
			}
			last_instantiation->PushCore(new SquareCore(ai));
		break;
		case st_RigidArm:
			last_instantiation->PushSection(new RigidArm());
		break;
		case st_Blaster:
			last_instantiation->PushSection(new Blaster());
			break;
		case st_HeatBeam:
			last_instantiation->PushSection(new HeatBeamGun());
			break;
		case st_SpinningJoint:
			{
				float deg_per_second = lua_tonumber(luaVM, 2);
				last_instantiation->PushSection(new SpinningJoint(deg_per_second));
			}
			break;
		default:
			return -1; //Not handled
			break;
	}
	return 0;
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

static int l_set_angle(lua_State * luaVM)
{
	assert(last_instantiation!=NULL);
	last_instantiation->SetAngle((float)lua_tonumber(luaVM, 1));
	return 0;
}
static int l_set_position(lua_State * luaVM)
{
	assert(last_instantiation!=NULL);
	last_instantiation->SetPosition((float)lua_tonumber(luaVM,1), (float)lua_tonumber(luaVM,2));
	return 0;
}



GameLua::GameLua(void)
{
	if(last_instantiation == NULL)
	{
		InitialiseMap(); //Allows me to use a big switch statement
		luaVM = lua_open();
		luaL_openlibs(luaVM);
		//Register all the lua functions
		lua_register(luaVM, "PushSection", l_push_section);
		lua_register(luaVM, "PopSection", l_pop);
		lua_register(luaVM, "AddAsEnemy", l_add_as_enemy);
		lua_register(luaVM, "AddAsFriend", l_add_as_friend);
		lua_register(luaVM, "SetAngle", l_set_angle);
		lua_register(luaVM, "SetPosition", l_set_position);
	}
	last_instantiation = this;
}

GameLua::~GameLua(void)
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

void GameLua::PushCore(Core* _core)
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

void GameLua::PushSection(Section* _section)
{
	if(!section_stack_.empty())
	{		
		section_stack_.top()->AddChild(_section);
		section_stack_.push(_section);		
	}
}

void GameLua::PopSection()
{
	if(section_stack_.size() > 1)
	{
		section_stack_.pop();
	}
}

void GameLua::AddAsFriend()
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

void GameLua::AddAsEnemy()
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

void GameLua::SetAngle(float _angle)
{
	section_stack_.top()->SetAngle(_angle);
}
void GameLua::SetPosition(float _x, float _y)
{
	section_stack_.top()->SetPosition(Vector3f(_x, _y, 0));
}

void GameLua::SetHealth(float _health)
{

}
std::list<Core_ptr>& GameLua::GetFriends()
{
	return friends_;	
}

std::list<Core_ptr>& GameLua::GetEnemies()
{
	return enemies_;	
}

lua_State* GameLua::GetLuaVM()
{
	return luaVM;
}

void GameLua::LoadShip(const char* ship)
{
	lua_getglobal(luaVM,"Ship");
	if(!lua_isnil(luaVM, -1))
	{
		lua_pushnil(luaVM);
		lua_setglobal(luaVM, "Ship");
	}
	lua_pop(luaVM,1); //Pops ship from stack

	luaL_dofile(luaVM, ship);	//Loads the ship function

	lua_getglobal(luaVM, "Ship");
	if(lua_istable(luaVM, -1))
	{
		//Get name
		//Load the rest
		LoadSections();
		lua_pop(luaVM, 1);
		this->AddAsFriend();
	}
	else
	{
		
	}

}

void GameLua::LoadSections()
{
	lua_pushstring(luaVM, "SectionType");
	lua_gettable(luaVM, -2);
	if(lua_isstring(luaVM, -1))
	{//Got SectionType, is required
		std::string section_type = lua_tostring(luaVM, -1);
		std::transform(section_type.begin(), section_type.end(), section_type.begin(), toupper);
		lua_pop(luaVM, 1);
		SectionType s_type = SectionMap[section_type];
		switch(s_type) //Get section custom parameters
		{
			{ //Cores
			BaseAI* ai;
			case st_SquareCore:
				lua_pushstring(luaVM, "AI");
				lua_gettable(luaVM, -2); //Pop AI and put AI table at top of stack
				if(lua_istable(luaVM, -1))
				{
					lua_pushstring(luaVM, "AIType");
					lua_gettable(luaVM, -2); //Pop AIType and puts the ai type on
					if(lua_isstring(luaVM, -1))
					{
						std::string ai_type_string = lua_tostring(luaVM, -1);
						std::transform(ai_type_string.begin(), ai_type_string.end(), ai_type_string.begin(), toupper);
						SectionType ai_type = SectionMap[ai_type_string];
						lua_pop(luaVM, 1); //Pop the AI type
						switch(ai_type)
						{
						case ai_RotatingAI:
							lua_pushstring(luaVM, "AIRotationRate");
							lua_gettable(luaVM, -2); //Puts rotation rate at top of stack
							if(lua_isnumber(luaVM, -1))
							{
								ai = new RotatingAI(lua_tonumber(luaVM, -1));
							}
							else
							{
								ai = new RotatingAI(0.2f); //Default to 20% rotation rate
							}
							lua_pop(luaVM, 1); //Pop the AI rotation rate
							break;
						case ai_KeyboardAI:
							ai = new KeyboardAI();
							break;
						default:
							ai = new RotatingAI(0.2f);
							break;
						}
						
					}
					else
					{
						ai = new RotatingAI(0.2f);
						lua_pop(luaVM, 1);
					}
					PushCore(new SquareCore(ai));
					lua_pop(luaVM, 1); //Pop the AI
				} else
				{
					lua_pop(luaVM, 1); //Probably have nill at top of stack, so pop
				}
				

				break;
			}
			case st_RigidArm:
				PushSection(new RigidArm());
				break;

		}


		lua_pushstring(luaVM, "Health");
		lua_gettable(luaVM, -2); //Puts any health value on the top of the stack
		if(lua_isnumber(luaVM, -1))
		{
			SetHealth(lua_tonumber(luaVM, -1));
		}
		lua_pop(luaVM, 1);

		lua_pushstring(luaVM, "Angle");
		lua_gettable(luaVM, -2); //Puts any health value on the top of the stack
		if(lua_isnumber(luaVM, -1))
		{
			SetAngle(lua_tonumber(luaVM, -1));
		}
		lua_pop(luaVM, 1);

		lua_pushstring(luaVM, "Position");
		lua_gettable(luaVM, -2); //Puts any health value on the top of the stack
		if(lua_istable(luaVM, -1))
		{
			float x = 0;
			float y = 0;
			lua_pushstring(luaVM, "x");
			lua_gettable(luaVM, -2); //Puts any health value on the top of the stack
			if(lua_isnumber(luaVM, -1))
			{
				x = lua_tonumber(luaVM, -1);
			}
			lua_pop(luaVM, 1);
			lua_pushstring(luaVM, "y");
			lua_gettable(luaVM, -2); //Puts any health value on the top of the stack
			if(lua_isnumber(luaVM, -1))
			{
				y = lua_tonumber(luaVM, -1);
			}

			SetPosition(x,y);
			lua_pop(luaVM, 1);
		}
		lua_pop(luaVM, 1);
		
		//Now load all the subsections
		lua_pushstring(luaVM, "SubSections");
		lua_gettable(luaVM, -2);
		if(lua_istable(luaVM, -1))
		{
			 int sub_section_count = luaL_getn(luaVM, -1);
			 for(int i = 1; i <= sub_section_count; i++)
			 {
				lua_rawgeti(luaVM, -1, i);
				LoadSections();
				PopSection();
				lua_pop(luaVM, 1);
			 }
		}
		lua_pop(luaVM, 1);
		
	}
}

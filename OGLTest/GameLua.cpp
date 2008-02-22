#include "StdAfx.h"
#include "GameLua.h"

#include "SquareCore.h"
#include "RigidArm.h"
#include "LongRigidArm.h"
#include "Blaster.h"
#include "HeatBeamGun.h"
#include "SpinningJoint.h"
#include "JointAngles.h"
#include "ProngLH.h"
#include "ProngRH.h"
#include "WidePlate.h"

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
	st_JointAngles,
	st_LongRigidArm,
	st_ProngLH,
	st_ProngRH,
	st_WidePlate,
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
	SectionMap["JOINTANGLES"] = st_JointAngles;
	SectionMap["LONGRIGIDARM"] = st_LongRigidArm;
	SectionMap["PRONGRH"] = st_ProngRH;
	SectionMap["PRONGLH"] = st_ProngLH;
	SectionMap["WIDEPLATE"] = st_WidePlate;
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

static int l_load_ship(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if(lua_isstring(luaVM, -1))
	{
		last_instantiation->LoadShip(lua_tostring(luaVM, -1));
	}
	else
	{
		//Report incorrect params
	}
	return 0;
}

static int l_set_color(lua_State* luaVM)
{
	assert(last_instantiation != NULL);
	if(lua_gettop(luaVM) == 3)
	{
		double r = lua_tonumber(luaVM, 1);
		double g = lua_tonumber(luaVM, 2);
		double b = lua_tonumber(luaVM, 3);
		//TODO check bounds here

		last_instantiation->SetColor(GLColor(r,g,b));
	}
	else
	{ 
		//Report incorrect params here
	}
	return 0;
}


static int l_scale_health(lua_State* luaVM)
{
	assert(last_instantiation != NULL);
	double scale = lua_tonumber(luaVM, -1);
	last_instantiation->ScaleHealth(scale);
	return 0;
}
static int l_override_ai(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	std::string ai_string = lua_tostring(luaVM, 1);
	std::transform(ai_string.begin(), ai_string.end(), ai_string.begin(), toupper);
	
	SectionType ai_type = SectionMap[ai_string];
	BaseAI* ai = NULL;
	switch(ai_type)
	{
	case ai_RotatingAI:
		{
		float rotRate = (float)lua_tonumber(luaVM, 2);
		ai = new RotatingAI(rotRate);
		}
		break;
	case ai_KeyboardAI:
		ai = new KeyboardAI();
		break;
	}
	if(ai != NULL)
		last_instantiation->OverrideAI(ai);
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
		lua_register(luaVM, "AddAsEnemy", l_add_as_enemy);
		lua_register(luaVM, "AddAsFriend", l_add_as_friend);
		lua_register(luaVM, "SetAngle", l_set_angle);
		lua_register(luaVM, "SetPosition", l_set_position);
		lua_register(luaVM, "SetColor", l_set_color);
		lua_register(luaVM, "LoadShip", l_load_ship);
		lua_register(luaVM, "ScaleHealth", l_scale_health);	
		lua_register(luaVM, "SetAI", l_override_ai);
	}
	last_instantiation = this;
	is_script_running_ = false; 
	sum_time_ = 0;
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
		StackToCore(); //Pop everything but the Core from the stack
		friends_.push_back(Core_ptr((Core*)section_stack_.top()));
		section_stack_.pop();
	}
}

void GameLua::AddAsEnemy()
{
	if(!section_stack_.empty())
	{
		StackToCore(); //Pop everything but the Core from the stack
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
	section_stack_.top()->SetMaxHealth(_health);
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
		ParseShip();
		lua_pop(luaVM, 1); //Pop Ship
		StackToCore(); //Pop everything but the Core from the stack
	}
	else
	{
		
	}

}

void GameLua::ParseShip()
{
	lua_pushstring(luaVM, "SectionType");
	lua_gettable(luaVM, -2);
	if(lua_isstring(luaVM, -1))
	{//Got SectionType, is required
		std::string section_type = lua_tostring(luaVM, -1);
		std::transform(section_type.begin(), section_type.end(), section_type.begin(), toupper);
		lua_pop(luaVM, 1);
		SectionType s_type = SectionMap[section_type];
		//TODO handle errors unknown section type
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
					lua_pop(luaVM, 1); //Probably have nil at top of stack, so pop
				}
				

				break;
			}
			case st_RigidArm:
				PushSection(new RigidArm());
				break;
			case st_Blaster:
				PushSection(new Blaster());
				break;
			case st_HeatBeam:
				PushSection(new HeatBeamGun());
				break;
			case st_SpinningJoint:
				{
				lua_pushstring(luaVM, "RotationRate");
				lua_gettable(luaVM, -2);
				float spin_rate_deg_per_sec = lua_tonumber(luaVM, -1);
				PushSection(new SpinningJoint(spin_rate_deg_per_sec));
				lua_pop(luaVM, 1);
				}
				break;
			case st_JointAngles:
				{
				lua_pushstring(luaVM, "FirstAngle");
				lua_gettable(luaVM, -2);
				float first_angle = lua_tonumber(luaVM, -1);
				lua_pop(luaVM, 1);
				lua_pushstring(luaVM, "SecondAngle");
				lua_gettable(luaVM, -2);
				float second_angle = lua_tonumber(luaVM, -1);
				lua_pop(luaVM, 1);
				lua_pushstring(luaVM, "TransitionTime");
				lua_gettable(luaVM, -2);
				float transition_time = lua_tonumber(luaVM, -1);
				lua_pop(luaVM, 1);
				lua_pushstring(luaVM, "PauseTime");
				lua_gettable(luaVM, -2);
				float pause_time = lua_tonumber(luaVM, -1);
				lua_pop(luaVM, 1);
				PushSection(new JointAngles(first_angle, second_angle, transition_time, pause_time));
				}
				break;
			case st_LongRigidArm:
				PushSection(new LongRigidArm());
				break;
			case st_ProngLH:
				PushSection(new ProngLH());
				break;
			case st_ProngRH:
				PushSection(new ProngRH());
				break;
			case st_WidePlate:
				PushSection(new WidePlate());
				break;
			default:
				//Handle spelling mistakes and whatnot.
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
				ParseShip();
				PopSection();
				lua_pop(luaVM, 1);
			 }
		}
		lua_pop(luaVM, 1);
		
	}
}


void GameLua::LoadChallenge(const char* challenge)
{
	lua_getglobal(luaVM,"Challenge");
	if(!lua_isnil(luaVM, -1))
	{
		lua_pushnil(luaVM);
		lua_setglobal(luaVM, "Challenge");
	}
	lua_pop(luaVM,1); //Pops challenge from stack

	if(!luaL_dofile(luaVM, challenge))
		is_script_running_ = true;	//Loads the challenge table
}

void GameLua::Tick(int _friend_count, int _enemy_count, float _timespan)
{
	sum_time_ += _timespan;
	lua_pushinteger(luaVM, _friend_count);
	lua_setglobal(luaVM, "FRIEND_COUNT");
	lua_pushinteger(luaVM, _enemy_count);
	lua_setglobal(luaVM, "ENEMY_COUNT");
	lua_pushnumber(luaVM, sum_time_);
	lua_setglobal(luaVM, "TOTAL_TIME");

	lua_getglobal(luaVM, "Challenge");
	if(lua_istable(luaVM, -1))
	{
		lua_pushstring(luaVM, "EntryPoint");
		lua_gettable(luaVM, -2);
		if(lua_isfunction(luaVM, -1))
		{
			lua_pcall(luaVM, 0, 1, 0); //Should pop EntryPoint and replace with either true or false
			if(lua_isboolean(luaVM, -1))
			{
				bool isDone = lua_toboolean(luaVM, -1);
				is_script_running_ = !isDone;
			}
			lua_pop(luaVM, 1); //Pop the boolean isDone
		}
		else
		{
			//Should report errors here
		}

		//ParseChallenge();
		lua_pop(luaVM, 1); //Pop the Challenge
	}
	else
	{
		lua_pop(luaVM, 1); //Pop what is probably nil
	}
}

void GameLua::StackToCore()
{
	while(section_stack_.size() > 1)
	{
		section_stack_.pop();
	}
}

void GameLua::OverrideAI(BaseAI* _AI)
{
	StackToCore();
	if(section_stack_.size()==1)
	{
		((Core*)section_stack_.top())->OverrideAI(_AI);
	}
	else
	{
		//TODO report errors
	}	
}

void GameLua::SetColor(GLColor _color)
{
	StackToCore();
	if(section_stack_.size()==1)
	{
		section_stack_.top()->SetColor(_color);
	}
	else
	{
		//TODO report errors
	}
}

void GameLua::ScaleHealth(float _scale)
{
		StackToCore();
	if(section_stack_.size()==1)
	{
		section_stack_.top()->ScaleHealth(_scale);
	}
	else
	{
		//TODO report errors
	}
}
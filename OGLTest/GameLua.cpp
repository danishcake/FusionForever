#include "StdAfx.h"
#include "GameLua.h"

#include "SquareCore.h"
#include "RigidArm.h"
#include "LongRigidArm.h"
#include "Blaster.h"
#include "HomingMissileLauncher.h"
#include "HeatBeamGun.h"
#include "SpinningJoint.h"
#include "JointAngles.h"
#include "ProngLH.h"
#include "ProngRH.h"
#include "WidePlate.h"
#include "Spike.h"
#include "SpikeLH.h"
#include "SpikeRH.h"
#include "ForwardWingLH.h"
#include "ForwardWingRH.h"
#include "LargeSquare.h"
#include "SmallSquare.h"
#include "NarrowProngLH.h"
#include "NarrowProngRH.h"
#include "QuarterCircle.h"
#include "SemiCircle.h"
#include "SweptWingLH.h"
#include "SweptWingRH.h"
#include "WingLH.h"
#include "WingRH.h"
#include "TinyCore.h"

#include "RotatingAI.h"
#include "KeyboardAI.h"

#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <string>
#include "GameScene.h"

static GameLua* last_instantiation = NULL;
lua_State* luaVM;

static enum SectionType
{
	unknown_key,
	st_SquareCore,
	st_TinyCore,
	st_RigidArm,
	st_Blaster,
	st_HomingMissileLauncher,
	st_HeatBeam,
	st_SpinningJoint,
	st_JointAngles,
	st_LongRigidArm,
	st_ProngLH,
	st_ProngRH,
	st_Spike,
	st_SpikeLH,
	st_SpikeRH,
	st_WidePlate,
	st_ForwardWingLH,
	st_ForwardWingRH,
	st_LargeSquare,
	st_SmallSquare,
	st_NarrowProngLH,
	st_NarrowProngRH,
	st_QuarterCircle,
	st_SemiCircle,
	st_SweptWingLH,
	st_SweptWingRH,
	st_WingLH,
	st_WingRH,
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
	SectionMap["HOMINGMISSILELAUNCHER"] = st_HomingMissileLauncher;
	SectionMap["SPIKE"] = st_Spike;	
	SectionMap["SPIKELH"] = st_SpikeLH;
	SectionMap["SPIKERH"] = st_SpikeRH;
	SectionMap["FORWARDWINGLH"] = st_ForwardWingLH;
	SectionMap["FORWARDWINGRH"] = st_ForwardWingRH;
	SectionMap["LARGESQUARE"] = st_LargeSquare;
	SectionMap["SMALLSQUARE"] = st_SmallSquare;
	SectionMap["NARROWPRONGLH"] = st_NarrowProngLH;
	SectionMap["NARROWPRONGRH"] = st_NarrowProngRH;
	SectionMap["QUARTERCIRCLE"] = st_QuarterCircle;
	SectionMap["SEMICIRCLE"] = st_SemiCircle;
	SectionMap["SWEPTWINGLH"] = st_SweptWingLH;
	SectionMap["SWEPTWINGRH"] = st_SweptWingRH;
	SectionMap["WINGLH"] = st_WingLH;
	SectionMap["WINGRH"] = st_WingRH;
	SectionMap["TINYCORE"] = st_TinyCore;
}


static std::string lua_stack_dump(lua_State* luaVM)
{
	int top = lua_gettop(luaVM);
	std::stringstream output;
	output << "Stack dump\n" << "Size of stack: " << top << "\n";
	for(int i = 1; i <= top; i++)
	{
		int t = lua_type(luaVM, i);
		switch (t) {    
			case LUA_TSTRING:  /* strings */
				output << "`" << lua_tostring(luaVM, i) << "'" << "\n";
				break;    
			case LUA_TBOOLEAN:  /* booleans */
				if(lua_toboolean(luaVM, i))
					output << "true" << "\n";
				else
					output << "false" << "\n";
				break;    
			case LUA_TNUMBER:  /* numbers */
				output << lua_tonumber(luaVM, i) << "\n";
				break;    
			default:  /* other values */
				output << lua_typename(luaVM, t) << "\n";
				break;    
		}
	}
	return output.str();
}

static int l_add_as_enemy(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if(lua_gettop(luaVM) == 0)
	{
		last_instantiation->AddAsEnemy();
	}
	else
	{
		luaL_error(luaVM, "AddAsEnemy must be called with no parameters");
	}
	return 0;
}

static int l_add_as_friend(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if(lua_gettop(luaVM) == 0)
	{
		last_instantiation->AddAsFriend();	
	}
	else
	{
		luaL_error(luaVM, "AddAsFriend must be called with no parameters");
	}
	return 0;
}

static int l_set_angle(lua_State * luaVM)
{
	assert(last_instantiation!=NULL);
	if((lua_gettop(luaVM) == 1) && (lua_isnumber(luaVM, -1)))
	{
		last_instantiation->SetAngle((float)lua_tonumber(luaVM, 1));
	}
	else
	{
		luaL_error(luaVM, "SetAngle must be called with 1 numeric parameter");
	}
	return 0;
}
static int l_set_position(lua_State * luaVM)
{
	assert(last_instantiation!=NULL);
	if((lua_gettop(luaVM) == 2) && (lua_isnumber(luaVM, -1)) && (lua_isnumber(luaVM, -2)))
	{
		last_instantiation->SetPosition((float)lua_tonumber(luaVM,1), (float)lua_tonumber(luaVM,2));
	}
	else
	{
		luaL_error(luaVM, "SetPosition must be called with 2 numeric parameters");
	}
	return 0;
}

static int l_load_ship(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if((lua_gettop(luaVM) == 1) && (lua_isstring(luaVM, -1)))
	{
		int ship_id = last_instantiation->LoadShip(lua_tostring(luaVM, -1));
		if(ship_id==-1)
		{
			luaL_error(luaVM,"LoadShip failed");
		}
		lua_pushnumber(luaVM, ship_id); //Push -1 in case of error, else ship Core section ID
	}
	else
	{
		luaL_error(luaVM, "LoadShip must be called with 1 string parameter");
	}
	return 1;
}

static int l_set_color(lua_State* luaVM)
{
	assert(last_instantiation != NULL);
	if((lua_gettop(luaVM) == 3) && (lua_isnumber(luaVM, -1)) &&
																 (lua_isnumber(luaVM, -2)) &&
																 (lua_isnumber(luaVM, -3)))
	{
		double r = lua_tonumber(luaVM, 1);
		double g = lua_tonumber(luaVM, 2);
		double b = lua_tonumber(luaVM, 3);
		//TODO check bounds here
		if((r >= 0.0 && r <= 1.0) &&
			 (r >= 0.0 && r <= 1.0) &&
			 (r >= 0.0 && r <= 1.0))
		{
			last_instantiation->SetColor(GLColor(r,g,b));
		} else
		{
			luaL_error(luaVM, "SetColor parameters must be in range 0 to 1);");
		}
	}
	else
	{
		luaL_error(luaVM, "SetColor must be called with 3 numeric parameters");
	}
	return 0;
}


static int l_scale_health(lua_State* luaVM)
{
	assert(last_instantiation != NULL);
	if((lua_gettop(luaVM) == 1) && (lua_isnumber(luaVM, -1)))
	{
		last_instantiation->ScaleHealth(static_cast<float>(lua_tonumber(luaVM, -1)));
	}
	else
	{
		luaL_error(luaVM, "ScaleHealth must be called with 1 numeric parameter");
	}
	return 0;
}

static int l_override_ai(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if(lua_gettop(luaVM) >= 1)
	{
		std::string ai_string = lua_tostring(luaVM, 1);
		std::transform(ai_string.begin(), ai_string.end(), ai_string.begin(), toupper);

		SectionType ai_type = SectionMap[ai_string];
		BaseAI* ai = NULL;
		switch(ai_type)
		{
		case ai_RotatingAI:
			{
				if(lua_gettop(luaVM) == 2)
				{
					float rotRate = (float)lua_tonumber(luaVM, 2);
					ai = new RotatingAI(rotRate);
				}
				else
				{
					luaL_error(luaVM, "SetAI(\"RotatingAI\"... expects a two parameters, the second being the rotation rate");
				}
			}
			break;
		case ai_KeyboardAI:
			if(lua_gettop(luaVM) == 1)
			{
				ai = new KeyboardAI();
			} else
			{
				luaL_error(luaVM, "SetAI(\"KeyboardAI\"... expects a one parameter");
			}
			break;
		default:
			luaL_error(luaVM, "SetAI must be called with one of the following AI types:\n(\"KeyboardAI\")\n(\"RotatingAI\",rot_rate)");
			break;
		}
		if(ai != NULL)
			last_instantiation->OverrideAI(ai);
	}
	else
	{
		luaL_error(luaVM, "IsAlive must be called with 1 or more parameters, depending on the AI type specified in the first parameter");
	}
	return 0;
}

static int l_is_alive(lua_State* luaVM)
{
	assert(last_instantiation!=NULL);
	if((lua_gettop(luaVM) == 1) && (lua_isnumber(luaVM, -1)))
	{
		int section_id = lua_tonumber(luaVM, -1);
		lua_pushboolean(luaVM, last_instantiation->IsAlive(section_id));
	}
	else
	{
		luaL_error(luaVM, "IsAlive must be called with 1 numeric parameter");
	}
	return 1;
}

static int l_log_error(lua_State* luaVM)
{
	if(lua_gettop(luaVM)==1 && lua_isstring(luaVM, -1))
	{
		Logger::LogError(lua_tostring(luaVM, -1));
	}
	else
	{
		lua_pop(luaVM, 1);
		Logger::LogError("Lua Error: LogError() must be called with a string parameter\n");
	}
	return 0;
}
static int l_log_to_screen(lua_State* luaVM)
{
	if(lua_gettop(luaVM)==1 && lua_isstring(luaVM, -1))
	{
		Logger::LogGameMessage(lua_tostring(luaVM, -1));
	}
	else
	{
		lua_pop(luaVM, 1);
		Logger::LogError("Lua Error: LogToScreen() must be called with a string parameter\n");
	}
	return 0;
}
static int l_log_diagnostic(lua_State* luaVM)
{
	if(lua_gettop(luaVM)==1 && lua_isstring(luaVM, -1))
	{
		Logger::Log(lua_tostring(luaVM, -1));
	}
	else
	{
		lua_pop(luaVM, 1); 
		Logger::LogError("Lua Error: Log() must be called with a string parameter\n");
	}
	return 0;
}
GameLua::GameLua(GameScene* _game_scene)
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
		lua_register(luaVM, "IsAlive", l_is_alive);
		lua_register(luaVM, "LogError", l_log_error);
		lua_register(luaVM, "LogToScreen", l_log_to_screen);
		lua_register(luaVM, "Log", l_log_diagnostic);
	}
	game_scene_ = _game_scene;
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

void GameLua::PushCore(Core_ptr _core)
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

void GameLua::PushSection(Section_ptr _section)
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
		friends_.push_back((Core_ptr)section_stack_.top());
		section_stack_.pop();
	}
}

void GameLua::AddAsEnemy()
{
	if(!section_stack_.empty())
	{
		StackToCore(); //Pop everything but the Core from the stack
		enemies_.push_back((Core_ptr)section_stack_.top());
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

int GameLua::LoadShip(const char* ship)
{
	lua_getglobal(luaVM,"Ship");
	if(!lua_isnil(luaVM, -1))
	{
		lua_pushnil(luaVM);
		lua_setglobal(luaVM, "Ship");
	}
	lua_pop(luaVM,1); //Pops ship from stack

	int run_result = luaL_dofile(luaVM, ship);	//Loads the ship function
	if(run_result == LUA_ERRRUN)
	{//Runtime error, should report and abandon
		Logger::LogError("GameLua::LoadShip: A runtime error occurred\n");
		return -1;
	} else if(run_result == LUA_ERRMEM)
	{
		Logger::LogError("GameLua::LoadShip: A memory allocation error occurred while running\n");
		return -1;
	} else if(run_result == LUA_ERRERR)
	{
		Logger::LogError("GameLua::LoadShip: Error handler error\n");
		return -1;
	} else
	{
		lua_getglobal(luaVM, "Ship");
		if(lua_istable(luaVM, -1))
		{
			//Get name
			//Load the rest
			ParseShip(ship);
			lua_pop(luaVM, 1); //Pop Ship
			StackToCore(); //Pop everything but the Core from the stack
			return section_stack_.top()->GetSectionID();
		}
		else
		{			
			Logger::LogError(std::string("GameLua::LoadShip: Unable to find Ship table in \n") + std::string(ship));
			return -1;
		}
	}
}

void GameLua::ParseShip(const char* _ship)
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
			case st_SquareCore:
				PushCore(new SquareCore(GetAI()));
				break;
			case st_TinyCore:
				PushCore(new TinyCore(GetAI()));
				break;
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
				if(lua_isnumber(luaVM, -1))
				{
					float spin_rate_deg_per_sec = static_cast<float>(lua_tonumber(luaVM, -1));
					PushSection(new SpinningJoint(spin_rate_deg_per_sec));
				}
				else
				{
					luaL_error(luaVM, "Error parsing %s\nSpinningJoints require a key:value pair RotationRate:number\n%s", _ship, lua_stack_dump(luaVM).c_str());
				}
				lua_pop(luaVM, 1);
				}
				break;
			case st_JointAngles:
				{
				bool param_error = false;
				lua_pushstring(luaVM, "FirstAngle");
				lua_gettable(luaVM, -2);
				float first_angle = static_cast<float>(lua_tonumber(luaVM, -1));
				param_error |= !lua_isnumber(luaVM, -1);
				lua_pop(luaVM, 1);

				lua_pushstring(luaVM, "SecondAngle");
				lua_gettable(luaVM, -2);
				float second_angle = static_cast<float>(lua_tonumber(luaVM, -1));
				param_error |= !lua_isnumber(luaVM, -1);
				lua_pop(luaVM, 1);

				lua_pushstring(luaVM, "TransitionTime");
				lua_gettable(luaVM, -2);
				float transition_time = static_cast<float>(lua_tonumber(luaVM, -1));
				param_error |= !lua_isnumber(luaVM, -1);
				lua_pop(luaVM, 1);

				lua_pushstring(luaVM, "PauseTime");
				lua_gettable(luaVM, -2);
				float pause_time = static_cast<float>(lua_tonumber(luaVM, -1));
				param_error |= !lua_isnumber(luaVM, -1);
				lua_pop(luaVM, 1);
				
				if(param_error)
					luaL_error(luaVM, "Error parsing %s\nJointAngles requires four key:value pairs:\nFirstAngle:number\nSecondAngle:number\nTransitionTime:number\nPauseTime:number", _ship);
				else
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
			case st_HomingMissileLauncher:
				PushSection(new HomingMissileLauncher());
				break;
			case st_Spike: 
				PushSection(new Spike());
				break;
			case st_SpikeLH:
				PushSection(new SpikeLH());
				break;
			case st_SpikeRH:
				PushSection(new SpikeRH());
				break;
			case st_ForwardWingLH:
				PushSection(new ForwardWingLH());
				break;
			case st_ForwardWingRH:
				PushSection(new ForwardWingRH());
				break;
			case st_LargeSquare:
				PushSection(new LargeSquare());
				break;
			case st_SmallSquare:
				PushSection(new SmallSquare());
				break;
			case st_NarrowProngLH:
				PushSection(new NarrowProngLH());
				break;
			case st_NarrowProngRH:
				PushSection(new NarrowProngRH());
				break;
			case st_QuarterCircle:
				PushSection(new QuarterCircle());
				break;
			case st_SemiCircle:
				PushSection(new SemiCircle());
				break;
			case st_SweptWingLH:
				PushSection(new SweptWingLH());
				break;
			case st_SweptWingRH:
				PushSection(new SweptWingRH());
				break;
			case st_WingLH:
				PushSection(new WingLH());
				break;
			case st_WingRH:
				PushSection(new WingRH());
				break;
			default:
				//Handle spelling mistakes and whatnot.
				luaL_error(luaVM, "Error parsing %s\nUnrecognised SectionType",_ship);
				break;
		}

		lua_pushstring(luaVM, "Delay");
		lua_gettable(luaVM, -2); //Puts any firing delay length on the top of the stack
		if(lua_isnumber(luaVM, -1))
			SetFiringDelay(static_cast<float>(lua_tonumber(luaVM, -1)));
		else if(!lua_isnil(luaVM, -1))
			Logger::Log(std::string("Nonfatal error in ")+std::string(_ship) + std::string(": Delay key must have a numeric value"));
		lua_pop(luaVM, 1);

		lua_pushstring(luaVM, "Health");
		lua_gettable(luaVM, -2); //Puts any health value on the top of the stack
		if(lua_isnumber(luaVM, -1))
			SetHealth(static_cast<float>(lua_tonumber(luaVM, -1)));
		else if(!lua_isnil(luaVM, -1))
			Logger::Log(std::string("Nonfatal error in ")+std::string(_ship) + std::string(": Health key must have a numeric value"));
		lua_pop(luaVM, 1);

		lua_pushstring(luaVM, "Angle");
		lua_gettable(luaVM, -2); //Puts any health value on the top of the stack
		if(lua_isnumber(luaVM, -1))
			SetAngle(static_cast<float>(lua_tonumber(luaVM, -1)));
		else if(!lua_isnil(luaVM, -1))
			Logger::Log(std::string("Nonfatal error in ")+std::string(_ship) + std::string(": Angle key must have a numeric value"));
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
				x = static_cast<float>(lua_tonumber(luaVM, -1));
			} else if(!lua_isnil(luaVM, -1))
			{
				Logger::Log(std::string("Nonfatal error in ")+std::string(_ship) + std::string(": Position.x key must have a numeric value"));		
			}
			lua_pop(luaVM, 1);
			lua_pushstring(luaVM, "y");
			lua_gettable(luaVM, -2); //Puts any health value on the top of the stack
			if(lua_isnumber(luaVM, -1))
			{
				y = static_cast<float>(lua_tonumber(luaVM, -1));
			} else if(!lua_isnil(luaVM, -1))
			{
				Logger::Log(std::string("Nonfatal error in ")+std::string(_ship) + std::string(": Position.y key must have a numeric value"));
			}

			SetPosition(x,y);
			lua_pop(luaVM, 1);
		}
		else if(!lua_isnil(luaVM, -1))
			Logger::Log(std::string("Nonfatal error in ")+std::string(_ship) + std::string(": Position key must have a table value"));
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
				ParseShip(_ship);
				PopSection();
				lua_pop(luaVM, 1);
			 }
		} else if(!lua_isnil(luaVM, -1))
		{
			Logger::Log(std::string("Nonfatal error in ")+std::string(_ship) + std::string(": SubSections key must have a table value"));
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
		Logger::Log("GameLua::LoadChallenge: Wiping previous challenge\n");
	}
	lua_pop(luaVM,1); //Pops challenge from stack
	 
	Logger::Log("GameLua::LoadChallenge: Attempting to load challenge\n");
	is_script_running_ = false;
	int load_result = luaL_loadfile(luaVM, challenge);

	if(load_result == LUA_ERRSYNTAX)
	{
		Logger::LogError("GameLua::LoadChallenge: A syntax error occurred while loading\n");
	} else if(load_result == LUA_ERRMEM)
	{
		Logger::LogError("GameLua::LoadChallenge: A memory allocation error occurred while loading\n");
	} else
	{//Loaded OK. Function ready to run at top of stack.
		int run_result = lua_pcall(luaVM, 0, LUA_MULTRET, 0);
		if(run_result == LUA_ERRRUN)
		{
			Logger::LogError("GameLua::LoadChallenge: A runtime error occurred\n");
			if(lua_isstring(luaVM, -1))
				Logger::LogError(lua_tostring(luaVM, -1));
		}
		else if(run_result == LUA_ERRMEM)
		{
			Logger::LogError("GameLua::LoadChallenge: A memory allocation error occurred while running\n");
			if(lua_isstring(luaVM, -1))
				Logger::LogError(lua_tostring(luaVM, -1));
		}
		else if(run_result == LUA_ERRERR)
		{
			Logger::Log("GameLua::LoadChallenge: Error handling function error\n");
			if(lua_isstring(luaVM, -1))
				Logger::LogError(lua_tostring(luaVM, -1));
		}
		else
		{//Everything worked OK, script loaded
			 is_script_running_ = true;
			 Logger::Log("GameLua::LoadChallenge: Challenge script loaded and run without problem\n");
		}
	}
}

void GameLua::Tick(int _friend_count, int _enemy_count, float _timespan)
{
	if(!is_script_running_)
		return;
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
			int run_result = lua_pcall(luaVM, 0, LUA_MULTRET, 0); //Should pop EntryPoint and replace with either true or false, then any error codes
			int parameter_count = lua_gettop(luaVM);
			if(run_result == LUA_ERRRUN)
			{//Runtime error, should report and abandon
				Logger::LogError("GameLua::Tick: A runtime error occurred\n");
				if(lua_isstring(luaVM, -1))
				{
					Logger::LogError(lua_tostring(luaVM, -1));
				}
				is_script_running_ = false;
			} else if(run_result == LUA_ERRMEM)
			{//Memory allocation error, should report and abandon
				Logger::LogError("GameLua::Tick: A memory allocation error occurred while running\n");
				if(lua_isstring(luaVM, -1))
				{
					Logger::LogError(lua_tostring(luaVM, -1));
				}
				is_script_running_ = false;
			} else if(run_result == LUA_ERRERR)
			{ 
				Logger::LogError("GameLua::Tick: Error handler error\n");
				if(lua_isstring(luaVM, -1))
				{
					Logger::LogError(lua_tostring(luaVM, -1));
				}
				is_script_running_ = false;
			} else
			{
				int pt = lua_type(luaVM, -1);
				if((parameter_count == 2) && lua_isboolean(luaVM, -1)) //Stack should be Challenge.isDone
				{
					int isDone = lua_toboolean(luaVM, -1);
					if(isDone)
						is_script_running_ = false;
				} else if((parameter_count == 2) && lua_isstring(luaVM, -1))
				{
					Logger::LogError("GameLua::Tick: EntryPoint returned an error\n");
					Logger::LogError(lua_tostring(luaVM, -1));
 					is_script_running_ = false;
				} else
				{
					Logger::LogError("GameLua::Tick: EntryPoint should return true or false, or an error string\n");
					is_script_running_ = false;
				}
				lua_pop(luaVM, 1); //Pop the boolean isDone
			}
		}
		else
		{
			Logger::LogError("GameLua::Tick: Unable to find EntryPoint function\n");
			is_script_running_ = false;
		}

		//ParseChallenge();
		lua_pop(luaVM, 1); //Pop the Challenge
	}
	else
	{
		Logger::LogError("GameLua::Tick: Unable to find Challenge table\n");
		lua_pop(luaVM, 1); //Pop what is probably nil
		is_script_running_ = false;
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
		((Core_ptr)section_stack_.top())->OverrideAI(_AI);
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

void GameLua::SetFiringDelay(float _firing_delay)
{
	section_stack_.top()->SetFiringDelay(_firing_delay);
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

bool GameLua::IsAlive(int _section_id)
{
	return this->game_scene_->IsSectionAlive(_section_id);
}
BaseAI* GameLua::GetAI()
{
	BaseAI* ai = NULL;
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
					ai = new RotatingAI(static_cast<float>(lua_tonumber(luaVM, -1)));
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
				luaL_error(luaVM, "Unrecognised AIType");
				break;
			}

		}
		else
		{
			//Report AIType must be string
			luaL_error(luaVM, "AI table require a string AIType property");
			lua_pop(luaVM, 1);
		}
		lua_pop(luaVM, 1); //Pop the AI table
	} else
	{
		luaL_error(luaVM, "Cores require an AI table property");
		lua_pop(luaVM, 1); //Probably have nil at top of stack, so pop
	}
	if(ai == NULL)
	{
		ai = new RotatingAI(0.2f);
	}
	return ai;
}



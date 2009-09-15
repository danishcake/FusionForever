#include "StdAfx.h"
#include "LuaChallenge.h"
#include "KeyboardAI.h"
#include "LuaAI.h"
#include "BaseGame.h"
#include <boost/algorithm/string.hpp>
#include "LuaTimeout.h"

//For lua based ship creation
#include "XMLCore.h"
#include "XMLSection.h"
#include "SectionTypes.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int LuaChallenge::l_SpawnShip(lua_State* _luaVM)
{
	//SpawnShip(self.ship_pointer, ship_name, force, position.x, position.y, angle, ai_script, health_scale)
	if(!(lua_gettop(_luaVM) == 8))
	{
		luaL_error(_luaVM, "SpawnShip must be called with 8 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -8)));
	assert(challenge);
	
	const char* ship_name = lua_tostring(_luaVM, -7);
	assert(ship_name);

	
	int force = static_cast<int>(lua_tointeger(_luaVM, -6));
	assert(force >= 0 && force < 8);
	Vector2f position = Vector2f(static_cast<float>(lua_tonumber(_luaVM, -5)), static_cast<float>(lua_tonumber(_luaVM, -4)));
	float angle = static_cast<float>(lua_tonumber(_luaVM, -3));

	const char* ai_script = lua_tostring(_luaVM, -2);
	assert(ai_script);
	
	float health_scale = static_cast<float>(lua_tonumber(_luaVM, -1));

	int ship_id = challenge->SpawnShip(std::string(ship_name), force, position, angle, std::string(ai_script), health_scale, _luaVM);

	lua_pushnumber(_luaVM, ship_id);

	return 1;
}

int LuaChallenge::SpawnShip(std::string _ship_name, int _force, Vector2f _position, float angle, std::string _ai_script, float _health_scale, lua_State* _luaVM)
{
	//Attempt to load ship
	Core_ptr core = Core::CreateCore(_ship_name);
	if(core)
	{
		core->SetPosition(Vector3f(_position.x, _position.y, 0));
		core->SetAngle(angle);
		core->SetColor(force_colors_[_force]);
		core->ScaleHealth(_health_scale);
		
		//Set the AI - either KeyboardAI or a LuaAI
		std::transform(_ai_script.begin(), _ai_script.end(), _ai_script.begin(), toupper);
		BaseAI* ai = NULL;
		if(boost::iequals(_ai_script, std::string("KEYBOARDAI")))
		{
			ai = new KeyboardAI();
		} else
		{
			ai = LuaAI::FromScript(_ai_script, &luaAIcache_);
			if(!ai)
			{
				delete core; 
				luaL_error(_luaVM, (std::string("Error loading AI:") + _ai_script).c_str());
			}
		}
		if(ai != NULL)
		{
			core->OverrideAI(ai);
		}
		game_->AddShip(core, _force);
	} else
	{
		luaL_error(_luaVM, (std::string("Unable to load Ship") + _ship_name).c_str());
	}
	return core->GetSectionID();
}

int LuaChallenge::l_GetShipsInArea(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 4))
	{
		luaL_error(_luaVM, "SpawnShip must be called with 4 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -4)));
	assert(challenge);
	float x = static_cast<float>(lua_tonumber(_luaVM, -3));
	float y = static_cast<float>(lua_tonumber(_luaVM, -2));
	float r = static_cast<float>(lua_tonumber(_luaVM, -1));

	lua_createtable(_luaVM, 0, 0); //Stack contains an empty table

	//Iterate over forces and build a table with index of all ships within circle
	int ship_index = 0;
	std::vector<Core_ptr> cores = challenge->GetShipsInArea(Vector3f(x, y, 0), r);
	BOOST_FOREACH(Core_ptr core, cores)
	{
		lua_pushinteger(_luaVM, ship_index);
		lua_pushinteger(_luaVM, core->GetSectionID());
		lua_settable(_luaVM, -3);
		ship_index++;
	}
	return 1;
}

std::vector<Core_ptr> LuaChallenge::GetShipsInArea(Vector3f _position, float _radius)
{
	return game_->GetShipsInArea(_position, _radius);
}


int LuaChallenge::l_KillShip(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 2))
	{
		luaL_error(_luaVM, "SpawnShip must be called with 2 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -2)));
	assert(challenge);

	int ship_id_target = static_cast<int>(lua_tointeger(_luaVM, -1));
	Core_ptr target = challenge->GetShipData(ship_id_target);
	target->TakeDamage(target->GetMaxHealth(), -1); //Anonymous damage
	return 0;
}

int LuaChallenge::l_luaError(lua_State* _luaVM)
{
	Logger::ErrorOut() << lua_tostring(_luaVM, -1) << "\n";
	
	return 0;
}

int LuaChallenge::l_SetHostility(lua_State* _luaVM)
{
	if(lua_gettop(_luaVM) != 4)
	{
		luaL_error(_luaVM, "SetHostility must be called with 4 parameters");
	}
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -4)));
	assert(challenge);

	int forceA = static_cast<int>(lua_tonumber(_luaVM, -3));
	int forceB = static_cast<int>(lua_tonumber(_luaVM, -2));

	if(forceA < 0 || forceA > MAX_FORCES)
		luaL_error(_luaVM, "ForceA should be between 0 and %d", MAX_FORCES);
	if(forceB < 0 || forceB > MAX_FORCES)
		luaL_error(_luaVM, "ForceB should be between 0 and %d", MAX_FORCES);

	bool hostile = (lua_toboolean(_luaVM, -1) != 0);

	challenge->SetHostility(forceA, forceB, hostile);
	return 0;
}

void LuaChallenge::SetHostility(int _force_a, int _force_b, bool _hostility)
{
	game_->SetHostility(_force_a, _force_b, _hostility);
}

int LuaChallenge::l_SetDeathFunction(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 3))
	{
		luaL_error(_luaVM, "SetDeathFunction must be called with 3 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -3)));
	assert(challenge);

	int ship_id = static_cast<int>(lua_tointeger(_luaVM, -2));
	int death_function_reference = luaL_ref(_luaVM, LUA_REGISTRYINDEX);
	Core_ptr core = static_cast<Core_ptr>(challenge->GetShipData(ship_id));
	core->SetDeathFunctionReference(death_function_reference);

	return 0;
}

int LuaChallenge::l_Victory(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 1))
	{
		luaL_error(_luaVM, "Victory must be called with just the ChallengePointer");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -1)));
	assert(challenge);
	challenge->DeclareVictory();
	return 0;
}

int LuaChallenge::l_Defeat(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 1))
	{
		luaL_error(_luaVM, "Defeat must be called with just the ChallengePointer");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -1)));
	assert(challenge);
	challenge->DeclareDefeat();
	return 0;
}

int LuaChallenge::l_Draw(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 1))
	{
		luaL_error(_luaVM, "Draw must be called with just the ChallengePointer");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -1)));
	assert(challenge);
	challenge->DeclareDraw();
	return 0;
}

int LuaChallenge::l_ReturnToEditor(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 1))
	{
		luaL_error(_luaVM, "ReturnToEditor must be called with just the ChallengePointer");
	}
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -1)));
	assert(challenge);
	challenge->ReturnToEditor();
	return 0;
}

int LuaChallenge::l_SetShipTarget(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 3))
	{
		luaL_error(_luaVM, "GetShipData must be called with 3 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -3)));
	assert(challenge);
	
	int ship_id_target = static_cast<int>(lua_tointeger(_luaVM, -1));
	Core_ptr target = challenge->GetShipData(ship_id_target);
	
	int ship_id_core = static_cast<int>(lua_tointeger(_luaVM, -2));
	Core_ptr core = challenge->GetShipData(ship_id_core);

	if(core && target)
	{
		core->GetAI()->SpecifyTarget(target);
	}
	if(!core)
		Logger::ErrorOut() << "Unable to find target with ID" << ship_id_target << "\n";
	if(!target)
		Logger::ErrorOut() << "Unable to find target with ID" << ship_id_core << "\n";

	return 0;
}

int LuaChallenge::l_GetShipData(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 2))
	{
		luaL_error(_luaVM, "GetShipData must be called with 2 parameters");
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -2)));
	assert(challenge);

	int ship_id = static_cast<int>(lua_tointeger(_luaVM, -1));

	Core_ptr core = challenge->GetShipData(ship_id);
	if(core != NULL)
	{
		lua_pushnumber(_luaVM, core->GetPosition().x);
		lua_pushnumber(_luaVM, core->GetPosition().y);
		lua_pushnumber(_luaVM, core->GetAngle());
		lua_pushboolean(_luaVM, true);
	} else
	{
		lua_pushnumber(_luaVM, 0);
		lua_pushnumber(_luaVM, 0);
		lua_pushnumber(_luaVM, 0);
		lua_pushboolean(_luaVM, false);
	}

	return 4;
}

Core* LuaChallenge::GetShipData(int _ship_id)
{
	return static_cast<Core_ptr>(game_->GetCoreData(_ship_id));
}

Section* LuaChallenge::GetSectionData(int _section_id)
{
	return game_->GetSectionData(_section_id);
}

int LuaChallenge::l_DisplayMessage(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 3))
	{
		luaL_error(_luaVM, "DisplayMessage must be called with 3 parameters");
	}
	float time = static_cast<float>(lua_tonumber(_luaVM, -1));
	std::string text = lua_tostring(_luaVM, -2);
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -3)));
	assert(challenge);

	challenge->DisplayMessage(text, time);

	return 0;
}

void LuaChallenge::DisplayMessage(std::string _message, float _time)
{
	game_->DisplayMessage(_message, _time + 0.5f);
}

int LuaChallenge::l_SetCounter(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 5))
	{
		luaL_error(_luaVM, "SetCounter must be called with 5 parameters");
	}
	int max = static_cast<int>(lua_tointeger(_luaVM, -1));
	int value = static_cast<int>(lua_tointeger(_luaVM, -2));
	bool visible = (lua_toboolean(_luaVM, -3) != 0);
	int counter_id = static_cast<int>(lua_tointeger(_luaVM, -4));
	if(counter_id < 1 || counter_id > 3)
	{
		luaL_error(_luaVM, "Counter must be between 1 and 3");
		return 0;
	}
	
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -5)));
	assert(challenge);

	challenge->SetCounter(counter_id, value, max, visible);
	return 0;
}

void LuaChallenge::SetCounter(int _counter, int _value, int _max, bool _visible)
{
	game_->SetCounter(_counter, _value, _max, _visible);
}

void LuaChallenge::ParseExistingShip(lua_State* _luaVM, Section* _section, int _childkey, int _stack_size)
{
	int start_stack_size = lua_gettop(_luaVM);
	//lua_newtable(_luaVM);													//Stack = BASE-table
	lua_pushnumber(_luaVM, _section->GetAngle());							//Stack = BASE-table-angle
	lua_setfield(_luaVM, -2, "angle");										//Stack = BASE-table
	lua_newtable(_luaVM);													//Stack = BASE-table-table
	lua_setfield(_luaVM, -2, "position");									//Stack = BASE-table
	lua_pushstring(_luaVM, "position");										//Stack = BASE-table-"position"
	lua_gettable(_luaVM, -2);												//Stack = BASE-table-position
	lua_pushnumber(_luaVM, _section->GetPosition().x);						//Stack = BASE-table-position-x
	lua_setfield(_luaVM, -2, "x");											//Stack = BASE-table-position
	lua_pushnumber(_luaVM, _section->GetPosition().y);						//Stack = BASE-table-position-y
	lua_setfield(_luaVM, -2, "y");											//Stack = BASE-table-position
	lua_pop(_luaVM, 1);														//Stack = BASE-table
	lua_pushstring(_luaVM, _section->GetSectionType().c_str());				//Stack = BASE-table-"BLASTERetc"
	lua_setfield(_luaVM, -2, "section_type");								//Stack = BASE-table

	lua_pushnumber(_luaVM, _childkey);										//Stack = BASE-table-childid
	lua_setfield(_luaVM, -2, "child_id");									//Stack = BASE-table

	lua_pushnumber(_luaVM, _section->GetSectionID());						//Stack = BASE-table-section_id
	lua_setfield(_luaVM, -2, "section_id");									//Stack = BASE-table

	lua_pushstring(_luaVM, "None");											//Stack = BASE-table-"none"
	lua_setfield(_luaVM, -2, "action");										//Stack = BASE-table

	lua_newtable(_luaVM);													//Stack = BASE-table-table
	lua_setfield(_luaVM, -2, "health");										//Stack = BASE-table
	lua_pushstring(_luaVM, "health");										//Stack = BASE-table-"health"
	lua_gettable(_luaVM, -2);												//Stack = BASE-table-health
	
	lua_pushboolean(_luaVM, 1);												//Stack = BASE-table-health-true
	lua_setfield(_luaVM, -2, "override");									//Stack = BASE-table-health
	
	lua_pushnumber(_luaVM, _section->GetMaxHealth());						//Stack = BASE-table-health-healthv
	lua_setfield(_luaVM, -2, "value");										//Stack = BASE-table-health
	lua_pop(_luaVM, 1);														//Stack = BASE-table
	
	/*
	if(_stack_size > 0)
	{
		lua_pushvalue(_luaVM, -4);											//Stack = BASE-table-parent
		lua_setfield(_luaVM, -2, "parent");
	}*/
	

	lua_newtable(_luaVM);													//Stack = BASE-table-table
	lua_setfield(_luaVM, -2, "subsections");								//Stack = BASE-table
	lua_pushstring(_luaVM, "subsections");									//Stack = BASE-table-"subsections"
	lua_gettable(_luaVM, -2);												//Stack = BASE-table-subsections


	
	//Now add subsections
	std::vector<Section_ptr> subsections = _section->GetSubsections();
	int child_key = 1;
	
	BOOST_FOREACH(Section_ptr section, subsections)
	{
		lua_pushnumber(_luaVM, child_key);									//Stack = BASE-table-subsections-key
		lua_newtable(_luaVM);												//Stack = BASE-table-subsections-key-section
		lua_settable(_luaVM, -3);											//Stack = BASE-table-subsections
		lua_pushnumber(_luaVM, child_key);									//Stack = BASE-table-subsections-key
		lua_gettable(_luaVM, -2);											//Stack = BASE-table-subsections-section
		lua_pushstring(_luaVM, "parent");									//Stack = BASE-table-subsections-section-"parent"
		lua_pushvalue(_luaVM, -4);											//Stack = BASE-table-subsections-section-"parent"-parent
		lua_settable(_luaVM, -3);											//Stack = BASE-table-subsections-section

		lua_remove(_luaVM, -3);												//Stack = BASE-subsections-section
		lua_remove(_luaVM, -2);												//Stack = BASE-section

		ParseExistingShip(_luaVM, section, child_key, _stack_size + 1);		//Stack = BASE-section
		//Restore stack to table-subsections
		lua_pushstring(_luaVM, "parent");									//Stack = BASE-section-"parent"
		lua_gettable(_luaVM, -2);											//Stack = BASE-section-parent
		lua_remove(_luaVM, -2);												//Stack = BASE-parent
		lua_pushstring(_luaVM, "subsections");								//Stack = BASE-parent-"subsections"
		lua_gettable(_luaVM, -2);											//Stack = BASE-parent-subsections

		child_key++;
	}
	lua_pop(_luaVM, 1);														//Stack = BASE-table
	lua_pushnumber(_luaVM, child_key);										//Stack = BASE-table-child_count
	lua_setfield(_luaVM, -2, "child_count");								//Stack = BASE-table

	lua_pushstring(_luaVM, "Section");
	lua_gettable(_luaVM, LUA_GLOBALSINDEX);
	lua_setmetatable(_luaVM, -2);

}

int LuaChallenge::l_GetDesign(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 2))
	{
		luaL_error(_luaVM, "GetCoreDesign must be called with 2 parameters");
	}
	if(!lua_isnumber(_luaVM, -1))
		luaL_error(_luaVM, "Ship id must be a number");
	int ship_id = lua_tointeger(_luaVM, -1);

	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -2)));
	assert(challenge);

	Core_ptr core = challenge->GetShipData(ship_id);
	if(core)
	{
		lua_newtable(_luaVM);														//Stack = BASE-unpopulated table
		ParseExistingShip(_luaVM, core, 0, 0);										//Stack = BASE-coredesign
	} else
	{
		Logger::DiagnosticOut() << "Unable to find a ship with ID " << ship_id << " to obtain design\n";
		lua_pushnil(_luaVM);
	}

	return 1;
}
int LuaChallenge::l_SpawnDesign(lua_State* _luaVM)
{
	//Stack = Challenge_ptr-core-position-angle-force-health_scale
	//Refered to as BASE from hereon
	if(!(lua_gettop(_luaVM) == 6))
	{
		luaL_error(_luaVM, "SpawnCoreDesign must be called with 2 parameters");
	}

	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -6)));
	assert(challenge);

	//Need to build the ship here
	Core_ptr core = NULL;

	if(!lua_isnumber(_luaVM, -1))
		luaL_error(_luaVM, "Health-scale (param 6) should be numeric, not %s", lua_typename(_luaVM, lua_type(_luaVM, -1)));
	if(!lua_isnumber(_luaVM, -2))
		luaL_error(_luaVM, "Force (param 5) should be numeric, not %s", lua_typename(_luaVM, lua_type(_luaVM, -2)));
	if(!lua_isnumber(_luaVM, -3))
		luaL_error(_luaVM, "Angle (param 4) should be numeric, not %s", lua_typename(_luaVM, lua_type(_luaVM, -3)));
	if(!lua_istable(_luaVM, -4))
		luaL_error(_luaVM, "Position (param 3) should be a table, not %s", lua_typename(_luaVM, lua_type(_luaVM, -4)));
	if(!lua_istable(_luaVM, -5))
		luaL_error(_luaVM, "Core (param 2) should be a table, not %s", lua_typename(_luaVM, lua_type(_luaVM, -5)));
	


	float health_scale = lua_tonumber(_luaVM, -1);								//Stack = BASE
	int force = lua_tointeger(_luaVM, -2);										//Stack = BASE
	float angle = lua_tonumber(_luaVM, -3);										//Stack = BASE

																				//Position at -4
	Vector3f position;
	lua_pushstring(_luaVM, "x");												//Stack = BASE-"x"
	lua_gettable(_luaVM, -5);													//Stack = BASE-x
	if(!lua_isnumber(_luaVM, -1))
		luaL_error(_luaVM, "param 3 should be a table with a numeric x key");
	position.x = lua_tonumber(_luaVM, -1);										//Stack = BASE-x
	lua_pop(_luaVM, 1);															//Stack = BASE

	lua_pushstring(_luaVM, "y");												//Stack = BASE-"y"
	lua_gettable(_luaVM, -5);													//Stack = BASE-y
	if(!lua_isnumber(_luaVM, -1))
		luaL_error(_luaVM, "param 3 should be a table with a numeric y key");
	position.y = lua_tonumber(_luaVM, -1);										//Stack = BASE-y
	lua_pop(_luaVM, 1);															//Stack = BASE

	std::stack<std::vector<int>> indices;
	lua_pushvalue(_luaVM, -5);
	ParseLuaShip(_luaVM, (Section**)&core,  false, challenge, indices);
	lua_pop(_luaVM, 1);
	core->SetPosition(position);
	core->SetAngle(angle);
	core->ScaleHealth(health_scale);
	int ship_id =  challenge->SpawnDesign(core, force);
	lua_pushinteger(_luaVM, ship_id);
	return 1;
}

void LuaChallenge::ParseLuaShip(lua_State* _luaVM, Section** _parent, bool _update_mode, LuaChallenge* _challenge, std::stack<std::vector<int>>& indices)
{
	int start_stack = lua_gettop(_luaVM);
	/* Parse section type */
	lua_pushstring(_luaVM, "section_type");										//Stack = BASE-"section_type"
	lua_gettable(_luaVM, -2);													//Stack = BASE-section_type
	std::string section_type = lua_tostring(_luaVM, -1);						//Stack = BASE-section_type
	lua_pop(_luaVM, 1);															//Stack = BASE

	/* Parse position */
	lua_pushstring(_luaVM, "position");											//Stack = Base-"position"
	lua_gettable(_luaVM, -2);													//Stack = Base-position
	Vector3f position;

	lua_pushstring(_luaVM, "x");												//Stack = BASE-position-"x"
	lua_gettable(_luaVM, -2);													//Stack = BASE-position-x
	position.x = lua_tonumber(_luaVM, -1);										//Stack = BASE-position-x
	lua_pop(_luaVM, 1);															//Stack = BASE

	lua_pushstring(_luaVM, "y");												//Stack = BASE-position-"y"
	lua_gettable(_luaVM, -2);													//Stack = BASE-position-y
	position.y = lua_tonumber(_luaVM, -1);										//Stack = BASE-position-y
	lua_pop(_luaVM, 2);															//Stack = BASE
	
	/* Parse angle */
	lua_pushstring(_luaVM, "angle");											//Stack = BASE-"angle"
	lua_gettable(_luaVM, -2);													//Stack = BASE-angle
	float angle = lua_tonumber(_luaVM, -1);										//Stack = BASE-angle
	lua_pop(_luaVM, 1);															//Stack = BASE

	/* Parse health */
	lua_pushstring(_luaVM, "health");											//Stack = BASE-"health"
	lua_gettable(_luaVM, -2);													//Stack = BASE-health
	lua_pushstring(_luaVM, "override");											//Stack = BASE-health-"override"
	lua_gettable(_luaVM, -2);													//Stack = BASE-health-override
	bool override_health = lua_toboolean(_luaVM, -1);							//Stack = BASE-health-override
	lua_pop(_luaVM, 1);
	lua_pushstring(_luaVM, "value");											//Stack = BASE-health-"value"
	lua_gettable(_luaVM, -2);													//Stack = BASE-health-value
	float health = lua_tonumber(_luaVM, -1);									//Stack = BASE-health-value
	lua_pop(_luaVM, 2);															//Stack = BASE

	lua_pushstring(_luaVM, "action");											//Stack = BASE-"action"
	lua_gettable(_luaVM, -2);													//Stack = BASE-action
	std::string action = lua_tostring(_luaVM, -1);								//Stack = BASE-action
	lua_pop(_luaVM, 1);															//Stack = BASE

	lua_pushstring(_luaVM, "section_id");										//Stack = BASE-"section_id"
	lua_gettable(_luaVM, -2);													//Stack = BASE-section_id
	int section_id = lua_tointeger(_luaVM, -1)	;								//Stack = BASE-section_id
	lua_pop(_luaVM, 1);															//Stack = BASE

	Section* current_section = NULL;
	if(_update_mode)
	{
		if(action == "Delete")
		{
			current_section = _challenge->GetSectionData(section_id);
			if(current_section)
			{
				current_section->SetMaxHealth(0);
			}
		} else if(action == "Create")
		{
			current_section = SectionTypes::GetSection(section_type);
			if(!current_section)
				current_section = XMLSection::CreateXMLSection(section_type);

			if(!current_section)
				luaL_error(_luaVM, "Unable to find %s in built in or XML sections", section_type);

			current_section->SetAngle(angle);
			current_section->SetPosition(position);
			if(override_health)
				current_section->SetMaxHealth(health);
			if(*_parent)
			{
				(*_parent)->AddChild(current_section);
			} else
			{
				luaL_error(_luaVM, "Attempted to create a section at the root - cannot do this, can only change");
			}
		} else // Just check for changes
		{
			current_section = _challenge->GetSectionData(section_id);
			if(current_section)
			{
				if(current_section->GetSectionType() != section_type)
				{
					std::vector<Section*> children = current_section->DetachChildren();
					std::vector<Section*> parent_children;
					if((*_parent))
					{
						parent_children = (*_parent)->DetachChildren();
					}
					parent_children.erase(std::remove(parent_children.begin(), parent_children.end(), current_section), parent_children.end());
					delete current_section;
					current_section = NULL;
								current_section = SectionTypes::GetSection(section_type);
					if(!current_section)
						current_section = XMLSection::CreateXMLSection(section_type);
					(*_parent)->AttachChildren(parent_children);
					if(!current_section)
					{
						//Clean up danglings
						BOOST_FOREACH(Section* section, children)
						{
							delete section;
						}
						luaL_error(_luaVM, "Unable to find %s in built in or XML sections", section_type);
					}
					if((*_parent))
					{
						(*_parent)->AddChild(current_section);
					}
					current_section->AttachChildren(children);
				}
				if(current_section->GetMaxHealth() != health && override_health)
				{
					current_section->SetMaxHealth(health);
				}
				if(current_section->GetAngle() != angle)
				{
					current_section->SetAngle(angle);
				}
				if(current_section->GetPosition() != position)
				{
					current_section->SetPosition(position);
				}

			}
		}
	} else
	{
		//Create section, attach to parent and populate fields.
		if(*_parent)
		{
			current_section = SectionTypes::GetSection(section_type);
			if(!current_section)
				current_section = XMLSection::CreateXMLSection(section_type);

			if(!current_section)
				luaL_error(_luaVM, "Unable to find %s in built in or XML sections", section_type);

			current_section->SetAngle(angle);
			current_section->SetPosition(position);
			if(override_health)
				current_section->SetMaxHealth(health);
			(*_parent)->AddChild(current_section);
		} else
		{
			*_parent = XMLCore::CreateXMLCore(section_type);
			if(!*_parent)
				luaL_error(_luaVM, "Unable to create core of type : %s", section_type);
			(*_parent)->SetAngle(angle);
			(*_parent)->SetPosition(position);
			if(override_health)
				(*_parent)->SetMaxHealth(health);
			current_section = *_parent;
		}
	}
	if(current_section)
	{
		/* Parse children and attach*/
		lua_pushstring(_luaVM, "subsections");							//Stack = Section-"subsections"
		lua_gettable(_luaVM, -2);										//Stack = Section-subsections[]

		/* get keys for subsection table (these are not guaranteed to be numeric, or without gaps */
		lua_pushnil(_luaVM);											//Stack = Section-subsections[]-nil
		indices.push(std::vector<int>());
		while(lua_next(_luaVM, -2))										//Stack = Section-subsections[]-key-subsection
		{
			//TODO error check type
			indices.top().push_back(lua_tonumber(_luaVM, -2));
			lua_pop(_luaVM, 1);											//Stack = Section-subsections[]-key
		}
		//lua_next pops key at end of loop automatically

		for(std::vector<int>::iterator it = indices.top().begin(); it != indices.top().end(); ++it)
		{
			lua_pushinteger(_luaVM, *it);									//Stack = Section-subsections[]-key
			lua_gettable(_luaVM, -2);										//Stack = Section-subsections[]-subsection
			lua_remove(_luaVM, -3);											//Stack = subsections[]-subsection
			lua_remove(_luaVM, -2);											//Stack = subsection
			ParseLuaShip(_luaVM, &current_section, _update_mode, _challenge, indices);
																			//Stack = Section
			lua_pushstring(_luaVM, "subsections");							//Stack = Section-"subsections"
			lua_gettable(_luaVM, -2);										//Stack = Section-subsections[]
		}
		lua_pop(_luaVM, 1);													//Stack = Section
		indices.pop();
		lua_pushstring(_luaVM, "parent");									//Stack = Section-"parent"
		lua_gettable(_luaVM, -2);											//Stack = Section-parent/nil
		lua_remove(_luaVM, -2);												//Stack = parent
	}
}


void RecursiveInfDepth(lua_State* _luaVM, std::stack<int>& indices)
{																	//Stack = Section
	/* Get section params */
	/* Get children */
	indices.push(1);
	lua_pushstring(_luaVM, "subsections");							//Stack = Section-"subsections"
	lua_gettable(_luaVM, -2);										//Stack = Section-subsections[]
	lua_pushinteger(_luaVM, indices.top());							//Stack = Section-subsections[]-1
	lua_gettable(_luaVM, -2);										//Stack = Section-subsections[]-subsection
	while(!lua_isnil(_luaVM, -1))
	{
		RecursiveInfDepth(_luaVM, indices);
		indices.top()++;
		lua_pop(_luaVM, 1);											//Stack = Section-subsections[]
		lua_pushinteger(_luaVM, indices.top());						//Stack = Section-subsections[]-n
		lua_gettable(_luaVM, -2);									//Stack = Section-subsections[]-subsection/nil
	}
	indices.pop();
	lua_pop(_luaVM, 1);												//Stack = Section
	lua_pushstring(_luaVM, "parent");								//Stack = Section-"parent"
	lua_gettable(_luaVM, -2);										//Stack = Section-parent
	lua_remove(_luaVM, -2);											//Stack = parent
}

int LuaChallenge::SpawnDesign(Core_ptr _core, int _force)
{
	game_->AddShip(_core, _force);
	_core->SetColor(force_colors_[_force]);
	return _core->GetSectionID();
}



int LuaChallenge::l_UpdateDesign(lua_State* _luaVM)
{
	/* Should be able to query this, obtain each section in turn and
	   modify them, delete them, or add them */

	if(!(lua_gettop(_luaVM) == 2))
	{
		luaL_error(_luaVM, "SpawnCoreDesign must be called with 2 parameters");
	}

	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -2)));
	assert(challenge);
	std::stack<std::vector<int>> indices;
	ParseLuaShip(_luaVM, NULL, TRUE, challenge, indices);
	return 0;
}




int LuaChallenge::l_GetSectionMetadataByType(lua_State* _luaVM)
{
	if(!(lua_gettop(_luaVM) == 2))
	{
		luaL_error(_luaVM, "GetSectionMetadataByType must be called with 2 parameters");
	}
	LuaChallenge* challenge = ((LuaChallenge*) (lua_touserdata(_luaVM, -2)));
	assert(challenge);
	std::string section_type = lua_tostring(_luaVM, -1);
	if(section_type.length() == 0)
		luaL_error(_luaVM, "GetSectionMetadataByType must be called with a section type. Empty string found");

	int a = lua_gettop(_luaVM);
	lua_newtable(_luaVM);					//ret
	lua_pushstring(_luaVM, "Tags");			//ret-"Tags"
	lua_newtable(_luaVM);					//ret-"Tags"-table
	std::vector<std::string> tags = SectionMetadata::GetTags(section_type);
	int tag_index = 1;
	for(std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ++it)
	{
		lua_pushinteger(_luaVM, tag_index++);	//ret-"Tags"-table-index
		lua_pushstring(_luaVM, it->c_str());	//ret-"Tags"-table-index-"tag"
		lua_settable(_luaVM, -3);				//ret-"Tags"-table
		Logger::DiagnosticOut() << "Returned a tag for section " << section_type << " of " << *it << "\n";
	}
	lua_settable(_luaVM, -3);					//ret
	bool c = lua_istable(_luaVM, -1);
	int b = lua_gettop(_luaVM);

	return 1;
}

LuaChallenge::LuaChallenge(lua_State* _luaVM, std::string _challenge, BaseGame* _game) : 
	luaVM_(_luaVM), challenge_(_challenge), game_(_game), luaAIcache_(luaVM_)
{
	lua_register(_luaVM, "SpawnShip", l_SpawnShip);
	lua_register(_luaVM, "Victory", l_Victory);
	lua_register(_luaVM, "Defeat", l_Defeat);
	lua_register(_luaVM, "Draw", l_Draw);
	lua_register(_luaVM, "ReturnToEditor", l_ReturnToEditor);
	lua_register(_luaVM, "GetShipData", l_GetShipData);
	lua_register(_luaVM, "GetShipsInArea", l_GetShipsInArea);
	lua_register(_luaVM, "_ALERT", l_luaError);
	lua_register(_luaVM, "SetDeathFunction", l_SetDeathFunction);
	lua_register(_luaVM, "SetShipTarget", l_SetShipTarget);
	lua_register(_luaVM, "SetHostility", l_SetHostility);
	lua_register(_luaVM, "DisplayMessage", l_DisplayMessage);
	lua_register(_luaVM, "SetCounter", l_SetCounter);
	lua_register(_luaVM, "KillShip", l_KillShip);

	lua_register(_luaVM, "SpawnDesign", l_SpawnDesign);
	lua_register(_luaVM, "UpdateDesign", l_UpdateDesign);
	lua_register(_luaVM, "GetDesign", l_GetDesign);

	lua_register(_luaVM, "GetSectionMetadataByType", l_GetSectionMetadataByType);

	state_ = ChallengeState::NotStarted;

	force_colors_[0] = GLColor(210, 0  , 0  );
	force_colors_[1] = GLColor(0  , 210, 0  );
	force_colors_[2] = GLColor(0  , 0  , 210);
	force_colors_[3] = GLColor(210, 210, 0  );
	force_colors_[4] = GLColor(0  , 210, 210);
	force_colors_[5] = GLColor(210, 0  , 210);
	force_colors_[6] = GLColor(50 , 50 , 50 );
	force_colors_[7] = GLColor(205, 205, 205);
	
	sum_time_ = 0;

	chunk_reference_ = 0;
	environment_reference_ = 0;
	coroutine_reference_ = 0;

	monitor_thread_ = new LuaTimeout("Challenge script has hung after 5 seconds without yield\n", 5.0f);


	assert(lua_gettop(_luaVM) == 0);
	//Now load the challenge chunk
	if(loadChallenge() == false)
	{
		assert(lua_gettop(_luaVM) == 0); 
		state_ = ChallengeState::LoadError;
		Logger::ErrorOut() << "Unable to load challenge script: " << challenge_ << "\n";
	} else
	{
		assert(lua_gettop(_luaVM) == 0);
		if(createEnvironmentAndCoroutine() == false)
		{
			state_ = ChallengeState::LoadError;
			Logger::ErrorOut() << "Unable to create environment. Is Challenge_sandbox broken?\n";
		} else
		{
			state_ = ChallengeState::Running;
		}
	}
}

LuaChallenge::~LuaChallenge(void)
{
	if(coroutine_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, coroutine_reference_);
	if(environment_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, environment_reference_);
	if(chunk_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, chunk_reference_);
	delete monitor_thread_;
}

bool LuaChallenge::loadChallenge()
{
	int load_result = luaL_loadfile(luaVM_, (std::string("Scripts/Challenges/") + challenge_).c_str());
	if(load_result==0)
	{
		chunk_reference_ = luaL_ref(luaVM_, LUA_REGISTRYINDEX);
		Logger::ErrorOut() << "Challenge loaded OK\n";
		return true;
	} else
	{
		std::string error_string = lua_tostring(luaVM_, -1);
		lua_pop(luaVM_, 1);
		Logger::ErrorOut() << "Load Challenge script error: " << error_string << "\n";
		return false;
	}
}

bool LuaChallenge::createEnvironmentAndCoroutine()
{
	lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, chunk_reference_); //Gets the function ready for setfenv

	/* Sandbox a file*/
	int env_load_error = luaL_loadfile(luaVM_, "Challenge_sandbox.lua");
	//Either have error message or chunk on stack
	if(env_load_error != 0 )
	{
		Logger::ErrorOut() << lua_tostring(luaVM_, -1) << "\n";
		lua_pop(luaVM_, 2);
		return false;
	}

	//The environment has a reference to this class in it...
	lua_pushlightuserdata(luaVM_, this);
	
	int env_run_error = lua_pcall(luaVM_, 1, 1, NULL);
	if(env_run_error != 0 )
	{
		Logger::ErrorOut() << lua_tostring(luaVM_, -1) << "\n";
		lua_pop(luaVM_, 2); //Pop error message and chunk
		return false;
	}
	//Should now have a reference to the environment on stack
	environment_reference_ = luaL_ref(luaVM_, LUA_REGISTRYINDEX);
	lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, environment_reference_); //Gets the environment back

	lua_setfenv(luaVM_, -2);
	lua_pop(luaVM_, 1); //Pops chunk from stack
	assert(lua_gettop(luaVM_) == 0);


	lua_State* coroutine = lua_newthread(luaVM_);
	coroutine_reference_= luaL_ref(luaVM_, LUA_REGISTRYINDEX); //Stores the coroutine reference. (also pops it :( )

	lua_rawgeti(coroutine, LUA_REGISTRYINDEX, chunk_reference_); //Gets the function for coroutine.

	return true; //Stack coroutine now contains the chunk to be executed, and is stored at coroutine_reference_
}

ChallengeState::Enum LuaChallenge::Tick(float _timespan)
{
	if(state_ == ChallengeState::Running)
	{
		sum_time_ += _timespan;
		
		// obtain coroutine
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, coroutine_reference_);
		lua_State* thread = lua_tothread(luaVM_, -1);
		lua_pop(luaVM_, 1); //Stack = empty (Checked)

		// Obtain Environment, then update info
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, environment_reference_);		//Stack = env
		lua_pushstring(luaVM_, "challenge");								//Stack = env-"challenge"
		lua_gettable(luaVM_, -2); 											//Stack = env-challenge (Checked)

		lua_pushstring(luaVM_, "time");										//Stack = env-challenge-"time"
		lua_pushnumber(luaVM_, sum_time_);									//Stack = env-challenge-"time"-#sum_time
		lua_settable(luaVM_, -3);											//Stack = env-challenge (Checked)

		lua_pushstring(luaVM_, "enemy_count");								//Stack = env-challenge-"enemy_count"
		lua_gettable(luaVM_, -2); 											//Stack = env-challenge-enemy_count
		for(int force = 0; force <MAX_FORCES; force++)
		{
			lua_pushinteger(luaVM_, force);									//Stack = env-challenge-enemy_count-#force
			lua_pushinteger(luaVM_, game_->GetEnemyCount(force));			//Stack = env-challenge-enemy_count-#force-#enemy_count
			lua_settable(luaVM_, -3);										//Stack = env-challenge-enemy_count
		}
		lua_pop(luaVM_, 1);													//Stack = env-challenge
		lua_pushstring(luaVM_, "force_count");								//Stack = env-challenge-"force_count"
		lua_gettable(luaVM_, -2);											//Stack = env-challenge-force_count
		for(int force = 0; force <MAX_FORCES; force++)
		{
			lua_pushinteger(luaVM_, force);									//Stack = env-challenge-force_count-#force
			lua_pushinteger(luaVM_, game_->GetForceCount(force));			//Stack = env-challenge-force_count-#force-#force_count
			lua_settable(luaVM_, -3);										//Stack = env-challenge-force_count
		}
		lua_pop(luaVM_, 1);													//Stack = env-challenge
		lua_pushstring(luaVM_, "friend_count");								//Stack = env-challenge-"friend_count"
		lua_gettable(luaVM_, -2);											//Stack = env-challenge-friend_count
		for(int force = 0; force <MAX_FORCES; force++)
		{
			lua_pushinteger(luaVM_, force);									//Stack = env-challenge-friend_count-#force
			lua_pushinteger(luaVM_, game_->GetFriendCount(force));			//Stack = env-challenge-friend_count-#force-#friend_count
			lua_settable(luaVM_, -3);										//Stack = env-challenge-friend_count
		}

		lua_pop(luaVM_, 3);													//Stack = empty

		assert(lua_gettop(luaVM_) == 0);

		int resume_result = monitor_thread_->SafeResume(thread);;
		if(resume_result == LUA_YIELD)
		{
			//A normal yield
		} else if(resume_result == 0)
		{
			//The script is over - if no final state is reported then call it a draw
			if(state_ == ChallengeState::Running)
				state_= ChallengeState::Draw;
		} else
		{
			//The script encountered a runtime error, log it
			state_ = ChallengeState::RunError;
			Logger::ErrorOut() << lua_tostring(thread, -1) << "\n";
			game_->DisplayMessage(lua_tostring(thread, -1), 10);
			lua_pop(thread, 1); //Pops the error message
			DeclareDraw();
			assert(lua_gettop(thread) == 0);
		}
	}
	
	assert(lua_gettop(luaVM_) == 0);
	return state_;
}

void LuaChallenge::CallDeathFunction(int _death_function_refence)
{
	if(_death_function_refence != 0)
	{
		// obtain death function
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, _death_function_refence);
		//This function should have been created in the correct environment, and thus inheritted it.
		int run_result = lua_pcall(luaVM_, 0, 0, 0);
		if(run_result != 0)
		{
			Logger::ErrorOut() << "Error running death function" << boost::lexical_cast<std::string, int>(_death_function_refence) << "\n";
			Logger::ErrorOut() << "Error:" << lua_tostring(luaVM_, -1) << "\n";
			lua_pop(luaVM_, 1);
		}
		//Dispose of the function reference
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, _death_function_refence);
	}
}
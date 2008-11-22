#include "StdAfx.h"
#include "LuaAI.h"
#include "Core.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static int l_PickRandomTarget(lua_State* luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(luaVM, -1)));
	assert(instance);
	instance->PickRandomTarget();
	return 0;
}

void LuaAI::PickRandomTarget()
{
	pick_random_next = true;
}

static int l_PickClosestTarget(lua_State* luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(luaVM, -1)));
	assert(instance);
	instance->PickClosestTarget();
	return 0;
}

void LuaAI::PickClosestTarget()
{
	pick_closest_next = true;
}

static int l_SetMoveDirection(lua_State* luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(luaVM, -3)));
	assert(instance);
	float x = static_cast<float>(lua_tonumber(luaVM, -2));
	float y = static_cast<float>(lua_tonumber(luaVM, -1));
	instance->SetMoveDirection(x, y);
	return 0;
}

void LuaAI::SetMoveDirection(float _x, float _y)
{
	Vector3f v = Vector3f(_x, _y, 0);
	if(v.lengthSq() > 1)
		v.normalize();
	next_move_.dx_ = v.x;
	next_move_.dy_ = v.y;
}

static int l_SetTurnDirection(lua_State* luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(luaVM, -2)));
	assert(instance);
	float dtheta = static_cast<float>(lua_tonumber(luaVM, -1));
	instance->SetTurnDirection(dtheta);
	return 0;
}

void LuaAI::SetTurnDirection(float _dtheta)
{
	next_move_.dtheta_ = _dtheta;
	if(next_move_.dtheta_ > 1)
		next_move_.dtheta_ = 1;
	if(next_move_.dtheta_ < -1)
		next_move_.dtheta_ = -1;
}

static int l_SetAll(lua_State* luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(luaVM, -5)));
	assert(instance);

	float dx = static_cast<float>(lua_tonumber(luaVM, -4));
	float dy = static_cast<float>(lua_tonumber(luaVM, -3));
	float dtheta = static_cast<float>(lua_tonumber(luaVM, -2));
	bool firing = static_cast<bool>(lua_toboolean(luaVM, -1));
	instance->SetAll(dx, dy, dtheta, firing);
	return 0;
}

void LuaAI::SetAll(float _x, float _y, float _dtheta, bool _firing)
{
	next_move_.dx_ = _x;
	next_move_.dy_ = _y;
	next_move_.dtheta_ = _dtheta;
	next_move_.firing_ = _firing;
}



static int l_SetCameraPosition(lua_State* luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(luaVM, -3)));
	assert(instance);

	float x = static_cast<float>(lua_tonumber(luaVM, -2));
	float y = static_cast<float>(lua_tonumber(luaVM, -1));
	instance->SetCameraPosition(x, y);
	return 0;
}

void LuaAI::SetCameraPosition(float _x, float _y)
{
	Camera::Instance().SetCentre(_x, _y, CameraLevel::Intro);
	Camera::Instance().SetFocus(self_->GetPosition().x, self_->GetPosition().y, CameraLevel::Intro);
}

bool LuaAI::initialised_lua = false;
void LuaAI::RegisterLuaFunctions(lua_State* _luaVM)
{
	if(!initialised_lua)
	{
		lua_register(_luaVM, "SetMoveDirection", l_SetMoveDirection);
		lua_register(_luaVM, "SetTurnDirection", l_SetTurnDirection);
		lua_register(_luaVM, "SetAll", l_SetAll);
		lua_register(_luaVM, "PickRandomTarget", l_PickRandomTarget);
		lua_register(_luaVM, "PickClosestTarget", l_PickClosestTarget);
		lua_register(_luaVM, "SetCameraPosition", l_SetCameraPosition);
	}
	initialised_lua = true;
}

LuaAI::LuaAI(std::string _file_name, int _chunk_reference, lua_State* _luaVM)
{
	RegisterLuaFunctions(_luaVM);
	script_name_= _file_name;
	chunk_reference_ = _chunk_reference;
	lua_state_ = _luaVM;
	next_move_ = AIAction();
	coroutine_reference_ = 0;
	environment_reference_ = 0;
	initialise_coroutine();
	target_ = NULL;
	self_ = NULL;
	pick_random_next = false;
	pick_closest_next = false;
	sum_time_ = 0;
	ok_to_run_ = true;
}

LuaAI::~LuaAI(void)
{
	//Should remove reference so GC can remove coroutine
	if(coroutine_reference_)
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, coroutine_reference_);
	if(environment_reference_)
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, environment_reference_);
	if(chunk_reference_)
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, chunk_reference_);

	if(target_)
		target_->RemoveSubscriber(this);
}

bool LuaAI::initialise_coroutine()
{
	if(environment_reference_ == 0)
	{
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, chunk_reference_); //Gets the function ready for setfenv

		/* Sandbox a file*/
		int env_load_error = luaL_loadfile(lua_state_, "AI_sandbox.lua");
		//Either have error message or chunk on stack
		if(env_load_error != 0 )
		{
			Logger::Instance() << lua_tostring(lua_state_, -1);
			return false;
		}

		//The environment has a reference to this class in it...
		lua_pushlightuserdata(lua_state_, this);
		
		int env_run_error = lua_pcall(lua_state_, 1, 1, NULL);
		if(env_run_error != 0 )
		{
			Logger::Instance() << lua_tostring(lua_state_, -1);
			return false;
		}
		//Should now have a reference to the environment on stack
		environment_reference_ = luaL_ref(lua_state_, LUA_REGISTRYINDEX);
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, environment_reference_); //Gets the environment back

		lua_setfenv(lua_state_, -2);
		lua_pop(lua_state_, 1); //Pops chunk from stack
	}

	if(coroutine_reference_)
		luaL_unref(lua_state_, LUA_REGISTRYINDEX, coroutine_reference_);

	lua_State* coroutine = lua_newthread(lua_state_);
	coroutine_reference_= luaL_ref(lua_state_, LUA_REGISTRYINDEX); //Stores the coroutine reference. (also pops it :( )

	lua_rawgeti(coroutine, LUA_REGISTRYINDEX, chunk_reference_); //Gets the function for coroutine.

	return true; //Stack coroutine now contains the chunk to be executed, and is stored at coroutine_reference_
}

void LuaAI::resume_coroutine(Core_ptr _self)
{
	assert(lua_gettop(lua_state_) == 0);
	if(coroutine_reference_ && ok_to_run_)
	{
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, coroutine_reference_);
		lua_State* thread = lua_tothread(lua_state_, -1);
		lua_pop(lua_state_, 1);
		assert(lua_gettop(lua_state_) == 0);
		
		//Obtain ship object and update position etc
		lua_rawgeti(lua_state_, LUA_REGISTRYINDEX, environment_reference_);
		//Environment on stack
			lua_pushstring(lua_state_, "ship");
			lua_gettable(lua_state_, -2);
				lua_pushstring(lua_state_, "time");
					lua_pushnumber(lua_state_, sum_time_);
			lua_settable(lua_state_,-3);										//Sets ship.time to sum_time_
				lua_pushstring(lua_state_, "angle");
					lua_pushnumber(lua_state_, _self->GetAngle() );
			lua_settable(lua_state_, -3);										//Sets ship.angle to _self.GetAngle() (Deg)
				lua_pushstring(lua_state_, "position");
				lua_gettable(lua_state_, -2);
					lua_pushstring(lua_state_,"x");
						lua_pushnumber(lua_state_, _self->GetPosition().x);
				lua_settable(lua_state_, -3);									//Sets ship.position.x to x
					lua_pushstring(lua_state_,"y");
						lua_pushnumber(lua_state_, _self->GetPosition().y);	
				lua_settable(lua_state_, -3);									//Sets ship.position.y to y
			lua_pop(lua_state_, 1); //Pops position from stack

				lua_pushstring(lua_state_, "target");
				lua_gettable(lua_state_, -2);
					lua_pushstring(lua_state_, "valid");
						lua_pushboolean(lua_state_, target_ != NULL);
				lua_settable(lua_state_, -3);									//Sets ship.target.valid to valid/invalid
			if(target_)
			{
					lua_pushstring(lua_state_, "position");
					lua_gettable(lua_state_, -2);
						lua_pushstring(lua_state_,"x");
							lua_pushnumber(lua_state_, target_->GetPosition().x);
					lua_settable(lua_state_, -3);								//Sets ship.target.position.x target.x
						lua_pushstring(lua_state_,"y");
							lua_pushnumber(lua_state_, target_->GetPosition().y);
					lua_settable(lua_state_, -3);								//Sets ship.target.position.y target.y
				lua_pop(lua_state_, 1); //Pops position from stack
					lua_pushstring(lua_state_, "angle");
						lua_pushnumber(lua_state_, target_->GetAngle());
				lua_settable(lua_state_, -3);
			}
		lua_pop(lua_state_, 3); //Pops ship, target and environment from stack
		assert(lua_gettop(lua_state_) == 0);
		int resume_result = lua_resume(thread, 0);
		if(resume_result == LUA_YIELD)
		{
			//Logger::Instance() << "Coroutine yielded OK\n";
		} else if(resume_result == 0)
		{
			//Logger::Instance() << "Coroutine finished without errors\n";
			initialise_coroutine(); //Restart coroutine //TODO work out a way to keep the ship, rather than initialising a new one.
		} else
		{
			ok_to_run_ = false;
			Logger::Instance() << lua_tostring(thread, -1) << "\n";
		}
		assert(lua_gettop(thread) == 0);
		//lua_pop(lua_state_, 1); //Pops thread //Shouldn't be needed
	}
	assert(lua_gettop(lua_state_) == 0);
}

AIAction LuaAI::Tick(float _timespan, std::vector<Core_ptr>& _allies, std::vector<Core_ptr>& _enemies, Core_ptr _self)
{
	/* Clears the next move, the resumes the AI			*/
	/* which will call methods on the LuaAI, setting	*/
	/* the next_move_									*/

	//TODO add timer so script can keep track of time
	self_ = _self;
	next_move_ = AIAction();
	sum_time_ += _timespan;
	
	if(pick_closest_next)
	{
		pick_closest_next = false;
		if(target_ != NULL)
			target_->RemoveSubscriber(this);
		if(_enemies.size() > 0)
		{
			std::vector<Core_ptr> enemies_sorted_by_range = _enemies;
			std::sort(enemies_sorted_by_range.begin(), enemies_sorted_by_range.end(), RelativeRangeSort<Core_ptr, Core_ptr>(_self));
			target_ = _enemies[0];
			target_->AddSubscriber(this);
		} else
		{
			target_ = NULL;
		}
	}
	
	if(pick_random_next)
	{
		pick_random_next = false;
		if(target_ != NULL)
			target_->RemoveSubscriber(this);
		if(_enemies.size() > 0)
		{
			int index = Random::RandomIndex(static_cast<int>(_enemies.size()));
			target_ = _enemies[index];
			target_->AddSubscriber(this);
		} else
		{
			target_ = NULL;
		}
	}

	resume_coroutine(_self);
	next_move_.target_ = target_;
	return next_move_;
}

LuaAI* LuaAI::FromScript(std::string _file_name, lua_State *_luaVM)
{
	//This just checks the file can be loaded before instanciating a LuaAI
	int load_result = luaL_loadfile(_luaVM, (std::string("scripts/ai/") + _file_name).c_str());
	if(load_result==0)
	{
		int chunk_ref = luaL_ref(_luaVM, LUA_REGISTRYINDEX);
		Logger::Instance() << "AI loaded OK\n";
		return new LuaAI(_file_name, chunk_ref, _luaVM);
	} else
	{
		std::string error_string = lua_tostring(_luaVM, -1);
		lua_pop(_luaVM, 1);
		Logger::Instance() << "Load AI script error: " << error_string << "\n";
		return NULL;
	}
}

void LuaAI::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
	{
		target_ = NULL;
	}
}



#include "StdAfx.h"
#include "LuaAI.h"
#include "Core.h"
#include "LuaTimeout.h"
#include "SandboxCache.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

LuaTimeout* LuaAI::monitor_thread_ = NULL;

int LuaAI::l_PickRandomTarget(lua_State* _luaVM)
{
	if(lua_gettop(_luaVM) != 1)
	{
		luaL_error(_luaVM, "PickRandomTarget must be called with 1 parameter");
	}
	LuaAI* instance = ((LuaAI*)(lua_touserdata(_luaVM, -1)));
	assert(instance);
	instance->pick_random_next_ = true;
	return 0;
}

int LuaAI::l_PickClosestTarget(lua_State* _luaVM)
{
	if(lua_gettop(_luaVM) != 1)
	{
		luaL_error(_luaVM, "PickClosestTarget must be called with 1 parameter");
	}
	LuaAI* instance = ((LuaAI*)(lua_touserdata(_luaVM, -1)));
	assert(instance);
	instance->pick_closest_next_ = true;
	return 0;
}

int LuaAI::l_SetMoveDirection(lua_State* _luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(_luaVM, -3)));
	assert(instance);
	float x = static_cast<float>(lua_tonumber(_luaVM, -2));
	float y = static_cast<float>(lua_tonumber(_luaVM, -1));
	Vector3f v = Vector3f(x, y, 0);
	if(v.lengthSq() > 1)
		v.normalize();
	instance->next_move_.dx_ = v.x;
	instance->next_move_.dy_ = v.y;
	return 0;
}

int LuaAI::l_SetTurnDirection(lua_State* _luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(_luaVM, -2)));
	assert(instance);
	float dtheta = static_cast<float>(lua_tonumber(_luaVM, -1));

	instance->next_move_.dtheta_ = dtheta;
	if(instance->next_move_.dtheta_ > 1)
		instance->next_move_.dtheta_ = 1;
	if(instance->next_move_.dtheta_ < -1)
		instance->next_move_.dtheta_ = -1;

	return 0;
}

int LuaAI::l_SetAll(lua_State* _luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(_luaVM, -5)));
	assert(instance);

	float x = static_cast<float>(lua_tonumber(_luaVM, -4));
	float y = static_cast<float>(lua_tonumber(_luaVM, -3));
	float dtheta = static_cast<float>(lua_tonumber(_luaVM, -2));
	bool firing = lua_toboolean(_luaVM, -1) != 0;

	Vector3f v = Vector3f(x, y, 0);
	if(v.lengthSq() > 1)
		v.normalize();
	instance->next_move_.dx_ = v.x;
	instance->next_move_.dy_ = v.y;

	instance->next_move_.dtheta_ = dtheta;
	if(instance->next_move_.dtheta_ > 1)
		instance->next_move_.dtheta_ = 1;
	if(instance->next_move_.dtheta_ < -1)
		instance->next_move_.dtheta_ = -1;

	instance->next_move_.firing_ = firing;

	return 0;
}

int LuaAI::l_SetCameraPosition(lua_State* _luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(_luaVM, -3)));
	assert(instance);

	float x = static_cast<float>(lua_tonumber(_luaVM, -2));
	float y = static_cast<float>(lua_tonumber(_luaVM, -1));
	Camera::Instance().SetCentre(x, y, CameraLevel::Intro);
	Camera::Instance().SetFocus(instance->self_->GetPosition().x, instance->self_->GetPosition().y, CameraLevel::Intro);
	return 0;
}

int LuaAI::l_ChangeAI(lua_State* _luaVM)
{
	LuaAI* instance = ((LuaAI*)(lua_touserdata(_luaVM, -2)));
	assert(instance);
	std::string filename = lua_tostring(_luaVM, -1);
	filename = filename;
	instance->ChangeAI(filename);
	return 0;
}

void LuaAI::ChangeAI(std::string _file_name)
{
	//Should remove reference so GC can remove coroutine
	if(coroutine_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, coroutine_reference_);
	if(environment_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, environment_reference_);
	if(target_)
		target_->RemoveSubscriber(this);
	target_ = NULL;

	int chunk_reference = cache_->GetReference(_file_name);
	if(chunk_reference == LUA_NOREF)
	{
		int load_result = luaL_loadfile(luaVM_, _file_name.c_str());
		if(load_result == 0)
		{
			chunk_reference = cache_->AddReference(_file_name);
		} else
		{
			std::string error_string = lua_tostring(luaVM_, -1);
			lua_pop(luaVM_, 1);
			Logger::ErrorOut() << "Change AI script error: " << error_string << "\n";
			ok_to_run_ = false;
		}
	}

	if(ok_to_run_)
	{
		script_name_= _file_name;
		chunk_reference_ = chunk_reference;
		next_move_ = AIAction();
		coroutine_reference_ = 0;
		environment_reference_ = 0;
		initialise_coroutine();
		target_ = NULL;
		self_ = NULL;
		pick_random_next_ = false;
		pick_closest_next_ = false;
		sum_time_ = 0;
	}
}

void LuaAI::RegisterLuaFunctions(SandboxCache* _cache)
{
	lua_State* luaVM = _cache->GetLuaVM();

	lua_register(luaVM, "SetMoveDirection", l_SetMoveDirection);
	lua_register(luaVM, "SetTurnDirection", l_SetTurnDirection);
	lua_register(luaVM, "SetAll", l_SetAll);
	lua_register(luaVM, "PickRandomTarget", l_PickRandomTarget);
	lua_register(luaVM, "PickClosestTarget", l_PickClosestTarget);
	lua_register(luaVM, "SetCameraPosition", l_SetCameraPosition);
	lua_register(luaVM, "ChangeAI", l_ChangeAI);

	int env_load_error = luaL_loadfile(luaVM, "AI_sandbox.lua");
	//Either have error message or chunk on stack
	if(env_load_error != 0 )
	{
		Logger::ErrorOut() << lua_tostring(luaVM, -1) << "\n";
		lua_pop(luaVM, 1);
	} else
	{
		_cache->SetSandboxReference();
	}
	if(!monitor_thread_)
		monitor_thread_ = new LuaTimeout("AI timed out after 5 seconds without yield", 5);
}

LuaAI::LuaAI(std::string _file_name, int _chunk_reference, SandboxCache* _cache)
{
	script_name_= _file_name;
	chunk_reference_ = _chunk_reference;
	luaVM_ = _cache->GetLuaVM();
	cache_ = _cache;
	next_move_ = AIAction();
	coroutine_reference_ = 0;
	environment_reference_ = 0;
	ai_sandbox_reference_ = _cache->GetSandboxReference();
	initialise_coroutine();
	target_ = NULL;
	self_ = NULL;
	pick_random_next_ = false;
	pick_closest_next_ = false;
	sum_time_ = 0;
	ok_to_run_ = true;
}

LuaAI::~LuaAI(void)
{
	//Should remove reference so GC can remove coroutine
	if(coroutine_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, coroutine_reference_);
	if(environment_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, environment_reference_);

	//Don't remove chunk cooroutine, as it should be cached for next usage

	if(target_)
		target_->RemoveSubscriber(this);
}

bool LuaAI::initialise_coroutine()
{
	//Gets the chunk to sandbox, and the this pointer, and runs that through the sandboxing
	//code, ending with chunk-environment on stack. Chunk environment is then set, and the 
	if(environment_reference_ == 0)
	{
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, chunk_reference_); //Gets the function ready for setfenv

		/* Sandbox a file*/
		//If the reference is zero then there was some problem loading it earlier (in initialiseLua)
		if(ai_sandbox_reference_ == 0)
		{
			Logger::ErrorOut() << lua_tostring(luaVM_, -1) << "\n";
			return false;
		}
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, ai_sandbox_reference_);


		//The environment has a reference to this class in it...
		lua_pushlightuserdata(luaVM_, this);
		
		int env_run_error = lua_pcall(luaVM_, 1, 1, NULL);
		if(env_run_error != 0 )
		{
			Logger::ErrorOut() << lua_tostring(luaVM_, -1) << "\n";
			lua_pop(luaVM_, 2); //Pops error and chunk from stack
			return false;
		}
		//Should now have a reference to the environment on stack
		environment_reference_ = luaL_ref(luaVM_, LUA_REGISTRYINDEX);
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, environment_reference_); //Gets the environment back

		lua_setfenv(luaVM_, -2);
		lua_pop(luaVM_, 1); //Pops chunk from stack
	}

	if(coroutine_reference_)
		luaL_unref(luaVM_, LUA_REGISTRYINDEX, coroutine_reference_);

	lua_State* coroutine = lua_newthread(luaVM_);
	coroutine_reference_= luaL_ref(luaVM_, LUA_REGISTRYINDEX); //Stores the coroutine reference. (also pops it :( )

	lua_rawgeti(coroutine, LUA_REGISTRYINDEX, chunk_reference_); //Gets the function for coroutine.

	return true; //Stack coroutine now contains the chunk to be executed, and is stored at coroutine_reference_
}

void LuaAI::resume_coroutine(Core_ptr _self, float _timespan)
{
	assert(lua_gettop(luaVM_) == 0);
	if(coroutine_reference_ && ok_to_run_)
	{
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, chunk_reference_);
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, coroutine_reference_);
		lua_State* thread = lua_tothread(luaVM_, -1);
		lua_pop(luaVM_, 1);
		assert(lua_gettop(luaVM_) == 1);
		
		//Obtain ship object and update position etc
		lua_rawgeti(luaVM_, LUA_REGISTRYINDEX, environment_reference_);
		//Environment on stack
			lua_pushstring(luaVM_, "ship");
			lua_gettable(luaVM_, -2);
				lua_pushstring(luaVM_, "time");
					lua_pushnumber(luaVM_, sum_time_);
			lua_settable(luaVM_,-3);										//Sets ship.time to sum_time_
				lua_pushstring(luaVM_, "dtime");
					lua_pushnumber(luaVM_, _timespan);
			lua_settable(luaVM_,-3);										//Sets ship.dtime to _timespan
				lua_pushstring(luaVM_, "angle");
					lua_pushnumber(luaVM_, _self->GetAngle() );
			lua_settable(luaVM_, -3);										//Sets ship.angle to _self.GetAngle() (Deg)
				lua_pushstring(luaVM_, "position");
				lua_gettable(luaVM_, -2);
					lua_pushstring(luaVM_,"x");
						lua_pushnumber(luaVM_, _self->GetPosition().x);
				lua_settable(luaVM_, -3);									//Sets ship.position.x to x
					lua_pushstring(luaVM_,"y");
						lua_pushnumber(luaVM_, _self->GetPosition().y);	
				lua_settable(luaVM_, -3);									//Sets ship.position.y to y
			lua_pop(luaVM_, 1); //Pops position from stack
				lua_pushstring(luaVM_, "ship_health_max");
					lua_pushnumber(luaVM_, _self->GetTotalHealth());
			lua_settable(luaVM_, -3);
				lua_pushstring(luaVM_, "ship_damage");
					lua_pushnumber(luaVM_, _self->GetTotalDamage());
			lua_settable(luaVM_, -3);
				lua_pushstring(luaVM_, "core_health_max");
					lua_pushnumber(luaVM_, _self->GetMaxHealth());
			lua_settable(luaVM_, -3);
				lua_pushstring(luaVM_, "core_damage");
					lua_pushnumber(luaVM_, _self->GetMaxHealth()- _self->GetHealth());
			lua_settable(luaVM_, -3);
				lua_pushstring(luaVM_, "target");
				lua_gettable(luaVM_, -2);
					lua_pushstring(luaVM_, "valid");
						lua_pushboolean(luaVM_, target_ != NULL);
				lua_settable(luaVM_, -3);									//Sets ship.target.valid to valid/invalid
			if(target_)
			{
					lua_pushstring(luaVM_, "position");
					lua_gettable(luaVM_, -2);
						lua_pushstring(luaVM_,"x");
							lua_pushnumber(luaVM_, target_->GetPosition().x);
					lua_settable(luaVM_, -3);								//Sets ship.target.position.x target.x
						lua_pushstring(luaVM_,"y");
							lua_pushnumber(luaVM_, target_->GetPosition().y);
					lua_settable(luaVM_, -3);								//Sets ship.target.position.y target.y
				lua_pop(luaVM_, 1); //Pops position from stack
					lua_pushstring(luaVM_, "angle");
						lua_pushnumber(luaVM_, target_->GetAngle());
				lua_settable(luaVM_, -3);
			}
		lua_pop(luaVM_, 2); //Pops ship, target from stack
		lua_setfenv(luaVM_, -2);
		lua_pop(luaVM_, 1);

		assert(lua_gettop(luaVM_) == 0);
		int resume_result = monitor_thread_->SafeResume(thread);
		if(resume_result == LUA_YIELD)
		{
		} else if(resume_result == 0)
		{
			initialise_coroutine(); //Restart coroutine
		} else
		{
			ok_to_run_ = false;
			Logger::ErrorOut() << script_name_ << " encountered an error: "<< lua_tostring(thread, -1) << "\n";
			lua_pop(thread, 1); //Pops the error message
			
		}
		assert(lua_gettop(thread) == 0);
	}
	assert(lua_gettop(luaVM_) == 0);
}

AIAction LuaAI::Tick(float _timespan, std::vector<Core_ptr>& /*_allies*/, std::vector<Core_ptr>& _enemies, Core_ptr _self)
{
	/* Clears the next move, the resumes the AI			*/
	/* which will call methods on the LuaAI, setting	*/
	/* the next_move_									*/


	self_ = _self;
	next_move_ = AIAction();
	sum_time_ += _timespan;
	
	if(pick_closest_next_)
	{
		pick_closest_next_ = false;
		if(target_ != NULL)
			target_->RemoveSubscriber(this);
		if(_enemies.size() > 0)
		{
			std::vector<Core_ptr> enemies_sorted_by_range = _enemies;
			std::sort(enemies_sorted_by_range.begin(), enemies_sorted_by_range.end(), RelativeRangeSort<Core_ptr, Core_ptr>(_self));
			target_ = enemies_sorted_by_range[0];
			target_->AddSubscriber(this);
		} else
		{
			target_ = NULL;
		}
	}
	
	if(pick_random_next_)
	{
		pick_random_next_ = false;
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

	resume_coroutine(_self, _timespan);
	next_move_.target_ = target_;
	return next_move_;
}

void LuaAI::SpecifyTarget(Core* _target)
{
	if(target_ != NULL)
		target_->RemoveSubscriber(this);
	target_ = _target;
	target_->AddSubscriber(this);
}

LuaAI* LuaAI::FromScript(std::string _file_name, SandboxCache* _cache)
{
	if(_cache->GetSandboxReference() == LUA_NOREF)
	{
		//Never been initialised, so do initialisation here
		RegisterLuaFunctions(_cache);
	}

	//Load a cached version, or if not cached then cache it
	int cached_chunk = _cache->GetReference(_file_name);
	if(cached_chunk != LUA_NOREF)
	{
		return new LuaAI(_file_name, cached_chunk, _cache);
	} else
	{
		int load_result = luaL_loadfile(_cache->GetLuaVM(), _file_name.c_str());
		if(load_result==0)
		{
			cached_chunk = _cache->AddReference(_file_name);
			return new LuaAI(_file_name, cached_chunk, _cache);
		} else
		{
			std::string error_string = lua_tostring(_cache->GetLuaVM(), -1);
			lua_pop(_cache->GetLuaVM(), 1);
			Logger::ErrorOut() << "Load AI script error: " << error_string << "\n";
			return NULL;
		}
	}
}

void LuaAI::EndSubscription(Subscriber* _source)
{
	if(target_ == _source)
	{
		target_ = NULL;
	}
}

local func_name, owner_pointer = ...;

--This sets up a sandbox for a LuaAI to run in.

--This is set up in the global scope, so grab the necessary functions
local ship_ = 
{
	_coroutine = coroutine,
	_print = print,
	_math = math,
	position = {x=0, y=0},
	angle = 0,
	ship_pointer = owner_pointer,
	time = 0,
	target = {position = {x=0, y=0}, valid = false, angle=0},
	_SetMoveDirection = SetMoveDirection,
	SetMoveDirection = function(x, y)
		_SetMoveDirection(ship_pointer, x, y)
	end,
	_SetTurnDirection = SetTurnDirection,
	SetTurnDirection = function(dtheta)
		_SetTurnDirection(ship_pointer, dtheta)
	end,
	_SetAll = SetAll,
	SetAll = function(dx, dy, dtheta, firing)
		_SetAll(ship_pointer, dx, dy, dtheta, firing)
	end,
	_PickRandomTarget = PickRandomTarget,
	PickRandomTarget = function()
		_PickRandomTarget(ship_pointer)
	end,
	_PickClosestTarget = PickClosestTarget,
	PickClosestTarget = function()
		_PickClosestTarget(ship_pointer)
	end,
	_SetCameraPosition = SetCameraPosition,
	SetCameraPosition = function(x, y)
		_SetCameraPosition(ship_pointer, x, y)
	end,
	WaitFor = function(time_to_wait)
		local end_time = time+time_to_wait
		while time < end_time do
			_coroutine.yield()
		end
	end,
	WaitForAnd = function(time_to_wait, dothis)
		local end_time = time+time_to_wait
		while time < end_time do
			dothis()
			_coroutine.yield()
		end
	end,
	FleeFor = function(time_to_flee)
		local end_time = time + time_to_flee
		while time < end_time do
			if target.valid then
				local range = _math.sqrt ((target.position.x - position.x) * (target.position.x - position.x) + (target.position.y - position.y) * (target.position.y - position.y))
				local dx = (target.position.x - position.x) / range
				local dy = (target.position.y - position.y) / range
				local right_vector_dx = _math.cos(angle)
				local right_vector_dy = -_math.sin(angle)
				local dotp = dx * right_vector_dx + dy * right_vector_dy
				
				SetAll(-dx, -dy, -dotp, false)
			else
				PickClosestTarget()
			end
			_coroutine.yield()
		end
	end,
	FleeForAnd = function(time_to_flee, dothis)
		local end_time = time + time_to_flee
		while time < end_time do
			if target.valid then
				local range = _math.sqrt ((target.position.x - position.x) * (target.position.x - position.x) + (target.position.y - position.y) * (target.position.y - position.y))
				local dx = (target.position.x - position.x) / range
				local dy = (target.position.y - position.y) / range
				local right_vector_dx = _math.cos(angle)
				local right_vector_dy = -_math.sin(angle)
				local dotp = dx * right_vector_dx + dy * right_vector_dy
				
				SetAll(-dx, -dy, -dotp, false)
			else
				PickClosestTarget()
			end
			dothis()
			_coroutine.yield()
		end
	end,
	AttackFor = function(time_to_attack, min_range, max_range)
		local end_time = time + time_to_attack
		while time < end_time do
			if target.valid == true then
				local range = _math.sqrt ((target.position.x - position.x) * (target.position.x - position.x) + (target.position.y - position.y) * (target.position.y - position.y))
				local dx = (target.position.x - position.x) / range
				local dy = (target.position.y - position.y) / range
				local right_vector_dx = _math.cos(angle)
				local right_vector_dy = -_math.sin(angle)
				local dotp = dx * right_vector_dx + dy * right_vector_dy
				local firing = _math.abs(dotp) < 0.4

				if range < min_range then
					--Back off
					SetAll(-dx, -dy, dotp, firing)
				elseif range > max_range then
					--Advance
					SetAll(dx, dy, dotp, firing)
				else
					--Strafe
					SetAll(dy, -dx, dotp, firing)
				end
			else
				PickClosestTarget()
			end
			_coroutine.yield()
		end
	end,
	AttackForAnd = function(time_to_attack, min_range, max_range, dothis)
		local end_time = time + time_to_attack
		while time < end_time do
			if target.valid == true then
				local range = _math.sqrt ((target.position.x - position.x) * (target.position.x - position.x) + (target.position.y - position.y) * (target.position.y - position.y))
				local dx = (target.position.x - position.x) / range
				local dy = (target.position.y - position.y) / range
				local right_vector_dx = _math.cos(angle)
				local right_vector_dy = -_math.sin(angle)
				local dotp = dx * right_vector_dx + dy * right_vector_dy
				local firing = _math.abs(dotp) < 0.4

				if range < min_range then
					--Back off
					SetAll(-dx, -dy, dotp, firing)
				elseif range > max_range then
					--Advance
					SetAll(dx, dy, dotp, firing)
				else
					--Strafe
					SetAll(dy, -dx, dotp, firing)
				end
			else
				PickClosestTarget()
			end
			dothis()
			_coroutine.yield()
		end
	end,

}

local ship_mt = 
{
	print = print,
	__newindex = function (t, n, v)
		print ("Unable to change contents of ship")
	end
}

setmetatable(ship_, ship_mt)

for func in pairs(ship_) do 
	if type(ship_[func]) == "function" and string.sub(func, 0, 1) ~= "_" then 
		--print("Changing the function: ship_[" .. func .. "]")
		setfenv(ship_[func], ship_)
	end
end

--Return a table to be the environment
local env_cage = {
	io = {}, --Disable all IO calls
	os = {}, --Disable all OS calls
	debug = {}, --Disable all debug
	require = {}, --Disable requiring packages
	module = {}, --Disable all module stuff
	rawset = {}, --Disable stuff that bypasses index metatable
	rawget = {},
	rawequal = {},
	setenvf = {}, --Disable setting the global table
	setmetatable = {}, --Disable funky metatable
	package = {}, --Disable all package loading

	--These are safe, but should be prevented from being modified...
	coroutine = coroutine,
	table = table,
	math = math,
	string = string,
	print = print,
	ship = ship_,
}

local env_mt = {__index = env_cage}
local env = {}
setmetatable(env, env_mt)

--env is now a sandbox. 
func = loadfile(func_name)
setfenv(func, env)
cr = coroutine.create(func)
return cr, env

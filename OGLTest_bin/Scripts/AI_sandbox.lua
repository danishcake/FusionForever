local func_name, owner_pointer = ...;

--This sets up a sandbox for a LuaAI to run in.

--This is set up in the global scope, so grab the necessary functions
local ship_ = 
{
	position = {x=0, y=0},
	ship_pointer = owner_pointer,
	_SetMoveDirection = SetMoveDirection,
	SetMoveDirection = function(x, y)
		_SetMoveDirection(ship_pointer, x, y)
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
setfenv(ship_.SetMoveDirection, ship_)
ship_.position.x = 0
ship_.position.y = 0

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

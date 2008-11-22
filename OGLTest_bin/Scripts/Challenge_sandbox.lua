local owner_pointer = ...;
require "Vector"

--This sets up a the environment for a Challenge script to run in.
local challenge_ = 
{
	challenge_pointer = owner_pointer,
	enemy_count = {0,0,0,0,0,0,0,0},
	force_count = {0,0,0,0,0,0,0,0},
	friend_count = {0,0,0,0,0,0,0,0},
	time = 0,
}

function challenge_:SpawnShip(ship_name, force, position, angle, ai_script, health_scale)
	assert(type(ship_name) == "string", "ship_name must be a string")
	assert(type(force) == "number", "force must be a number")
	assert(type(position) == "table", "1 position must be a Vector")
	assert(type(position.x) == "number", "2 position must be a Vector")
	assert(type(position.y) == "number", "3 position must be a Vector")
	assert(type(angle) == "number", "angle must be a number")
	assert(type(ai_script) == "string", "ai_script must be a string")
	
	if health_scale ~= nil then 
		assert(type(health_scale) == "number", "If specified, health_scale must be a number")
		return SpawnShip(self.challenge_pointer, ship_name, force, position.x, position.y, angle, ai_script, health_scale)
	else
		return SpawnShip(self.challenge_pointer, ship_name, force, position.x, position.y, angle, ai_script, 1)
	end
end

function challenge_:WaitForAllEnemiesDead(force)
	assert(type(force) == "number", "force must be a number")
	assert(force >= 0 and force < 8, "force must be in range 0-7")
	
	coroutine.yield()
	while(self.enemy_count[force] > 0) do
		coroutine.yield()
	end
end

function challenge_:WaitForForceCountLessThan(force, less_than)
	assert(type(force) == "number", "force must be a number")
	assert(force >= 0 and force < 8, "force must be in range 0-7")
	assert(type(less_than) == "number", "less_than must be a number")
	assert(less_than > 0, "less_than must be > 0")
	
	coroutine.yield()
	while(self.force_count[force] >= less_than) do
		coroutine.yield()
	end
end

function challenge_:WaitForDeath(ship_id)
	coroutine.yield()
	local ship_data = self:GetShipData(ship_id)
	
	while ship_data.alive == true do
		coroutine.yield()
		ship_data = self:GetShipData(ship_id)
	end
end

function challenge_:WaitFor(period)
	local end_time = self.time + period
	while(self.time < end_time) do
		coroutine.yield()
	end
end

function challenge_:GetShipData(ship_id)
	local x, y, angle, alive = GetShipData(self.challenge_pointer, ship_id)
	local ship = {}
	ship.position = Vector:new(x, y)
	ship.angle = angle
	ship.alive = alive;
	return ship
end

function challenge_:Victory()
	Victory(self.challenge_pointer)
end

function challenge_:Defeat()
	Defeat(self.challenge_pointer)
end

local challenge_mt_ = 
{
	__newindex = function (t, n, v)
		print ("Unable to change contents of the challenge")
	end
}

setmetatable(challenge_, challenge_mt_)

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
	Vector = Vector,
	table = table,
	math = math,
	string = string,
	print = print,
	challenge = challenge_,
}

local env_mt = {__index = env_cage}
local env = {}
setmetatable(env, env_mt)

--env is now ready, needs to be set as environment for the challenge to be run in.
--Then a coroutine must be created.
return env

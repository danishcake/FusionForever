local owner_pointer = ...;
require "Vector"
require "ShipBuilder"
require "Path"

--Initialises the ship builder
builder_:init(owner_pointer)


--This sets up a the environment for a Challenge script to run in.
local challenge_ = 
{
	challenge_pointer = owner_pointer,
	enemy_count = {0,0,0,0,0,0,0,0},
	force_count = {0,0,0,0,0,0,0,0},
	friend_count = {0,0,0,0,0,0,0,0},
	time = 0,
	use_absolute_paths = false,
}

function challenge_:SpawnShip(ship_name, force, position, angle, ai_script, health_scale)
	assert(type(ship_name) == "string", "ship_name must be a string")
	assert(type(force) == "number", "force must be a number")
	assert(type(position) == "table", "1 position must be a Vector")
	assert(type(position.x) == "number", "2 position must be a Vector")
	assert(type(position.y) == "number", "3 position must be a Vector")
	assert(type(angle) == "number", "angle must be a number")
	assert(type(ai_script) == "string", "ai_script must be a string")

	
	if self.use_absolute_paths ~= true then
		if ai_script ~= "KeyboardAI" then ai_script = "Scripts/AI/" .. ai_script end
		ship_name = "Scripts/Ships/" .. ship_name .. ".xmlShip"
	end

	if health_scale ~= nil then 
		assert(type(health_scale) == "number", "If specified, health_scale must be a number")
		return SpawnShip(self.challenge_pointer, ship_name, force, position.x, position.y, angle, ai_script, health_scale)
	else
		return SpawnShip(self.challenge_pointer, ship_name, force, position.x, position.y, angle, ai_script, 1)
	end
end

function challenge_:SetHostility(forceA, forceB, hostile, mirrored)
	local mirror_hostility = true
	if mirrored ~= nil then
		assert(type(mirrored) == "boolean")
		mirror_hostility = mirrored
	end
	assert(type(forceA) == "number")
	assert(type(forceB) == "number")
	assert(type(hostile) == "boolean")
	assert(forceA > 0 and forceA < 8)
	assert(forceB > 0 and forceB < 8)
	assert(forceA ~= forceB)
	
	SetHostility(self.challenge_pointer, forceA, forceB, hostile)
	if mirrored_hostility then
		SetHostility(self.challenge_pointer, forceB, forceA, hostile)
	end
end

function challenge_:SetHostile(forceA, forceB)
	assert(type(forceA) == "number")
	assert(type(forceB) == "number")
	assert(forceA ~= forceB)
	SetHostility(self.challenge_pointer, forceA, forceB, true)
	SetHostility(self.challenge_pointer, forceB, forceA, true)
end

function challenge_:SetFriendly(forceA, forceB)
	assert(type(forceA) == "number")
	assert(type(forceB) == "number")
	assert(forceA ~= forceB)
	SetHostility(self.challenge_pointer, forceA, forceB, false)
	SetHostility(self.challenge_pointer, forceB, forceA, false)
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
	ship.alive = alive
	ship.ship_id = ship_id
	return ship
end

function challenge_:UpdateShipData(ship_table)
	local x, y, angle, alive = GetShipData(self.challenge_pointer, ship_table.ship_id)
	ship_table.position.x = x
	ship_table.position.y = y
	ship_table.angle = angle
	ship_table.alive = alive
end

function challenge_:SetShipTarget(ship_id, target_id)
	SetShipTarget(self.challenge_pointer, ship_id, target_id)
end

function challenge_:KillShip(ship_id)
	KillShip(self.challenge_pointer, ship_id)
end

function challenge_:GetShipsInArea(position, radius)
	local ship_index = GetShipsInArea(self.challenge_pointer, position.x, position.y, radius)
	local ships = {}
	for i,v in ipairs(ship_index) do
		table.insert(ships, self:GetShipData(v))
	end
	return ships;
end

function challenge_:ClearArea(position, radius)
	local ships = self:GetShipsInArea(position, radius)
	for i,v in ipairs(ships) do
		self:KillShip(v.ship_id)
	end
end

function challenge_:Inspect(ship_id)
	local shipdata = self:GetShipData(ship_id)
	coroutine.yield()
	PauseGame(self.challenge_pointer)
	
	local end_time = self.time + 5
	local start_time = self.time
	local camera = GetCamera(self.challenge_pointer)
	local cp = Vector:new(camera.cx, camera.cy)
	local fp = Vector:new(camera.fx, camera.fy)
	
	path = Path:new(false)
	path:addPoint(cp, 1)
	path:addPoint(shipdata.position + Vector:new(-10, 10), 3)
	path:addPoint(shipdata.position + Vector:new(10, -10), 0.5)
	path:addPoint(cp, 1)
	
	widthpath = Path:new(false)
	widthpath:addPoint(Vector:new(camera.w, 0), 1)
	widthpath:addPoint(Vector:new(300, 0), 3)
	widthpath:addPoint(Vector:new(400, 0), 0.5)
	widthpath:addPoint(Vector:new(camera.w, 0), 1)
	
		
	local last_time = self.time
	local labelled = false
	while(self.time < end_time) do
		path:tick(self.time - last_time)
		widthpath:tick(self.time - last_time)
		SetCamera(self.challenge_pointer, path:getPosition().x, path:getPosition().y, fp.x, fp.y, widthpath:getPosition().x)
		last_time = self.time
		if (self.time - start_time) > 0.5 and labelled == false then
			LabelShip(self.challenge_pointer, ship_id, 2)
			labelled = true		
		end
		
		
		coroutine.yield()
	end
	
	ResumeGame(self.challenge_pointer)
end

function challenge_:Victory()
	Victory(self.challenge_pointer)
end

function challenge_:Defeat()
	Defeat(self.challenge_pointer)
end

function challenge_:Draw()
	Draw(self.challenge_pointer)
end

function challenge_:ReturnToEditor()
	ReturnToEditor(self.challenge_pointer)
end

function challenge_:SetDeathFunction(ship_id, death_function)
	SetDeathFunction(self.challenge_pointer, ship_id, death_function);
end

function challenge_:DisplayMessage(text, time_to_display)
	assert(type(text) == "string", "text must be a string")
	assert(type(time_to_display) == "number", "time_to_display must be a number")
	DisplayMessage(self.challenge_pointer, text, time_to_display)
end

function challenge_:SetCounterA(visible, value, max)
	assert(type(visible) == "boolean", "visible must be a boolean")
	assert(type(value) == "number", "time_to_display must be a number")
	assert(type(max) == "number", "max must be a number")
	SetCounter(self.challenge_pointer, 1, visible, value, max)
end

function challenge_:SetCounterB(visible, value, max)
	assert(type(visible) == "boolean", "visible must be a boolean")
	assert(type(value) == "number", "time_to_display must be a number")
	assert(type(max) == "number", "max must be a number")
	SetCounter(self.challenge_pointer, 2, visible, value, max)
end

function challenge_:SetCounterC(visible, value, max)
	assert(type(visible) == "boolean", "visible must be a boolean")
	assert(type(value) == "number", "time_to_display must be a number")
	assert(type(max) == "number", "max must be a number")
	SetCounter(self.challenge_pointer, 3, visible, value, max)
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
	os = {time = os.time}, --Disable all OS calls
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
	error = error,
	challenge = challenge_,
	ipairs = ipairs,
	pairs = pairs,
	builder = builder_
}

local env_mt = {__index = env_cage}
local env = {}
setmetatable(env, env_mt)

--env is now ready, needs to be set as environment for the challenge to be run in.
--Then a coroutine must be created.
return env

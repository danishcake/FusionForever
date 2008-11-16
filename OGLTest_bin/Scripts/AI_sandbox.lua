local owner_pointer = ...;
require "Vector"

--This sets up a the environment for a LuaAI to run in.
local ship_ = 
{
	position = Vector:new(0, 0),
	angle = 10,
	ship_pointer = owner_pointer,
	time = 0,
	target = {position = Vector:new(0, 0), valid = false, angle=0},
}

function ship_:SetMoveDirection(x, y)
	SetMoveDirection(self.ship_pointer, x, y)
end

function ship_:SetTurnDirection(dtheta)
    SetTurnDirection(self.ship_pointer, dtheta)
end

function ship_:SetAll(dx, dy, dtheta, firing)
    SetAll(self.ship_pointer, dx, dy, dtheta, firing)
end

function ship_:PickRandomTarget()
    PickRandomTarget(self.ship_pointer)
end

function ship_:PickClosestTarget()
    PickClosestTarget(self.ship_pointer)
end

function ship_:SetCameraPosition(x, y)
    SetCameraPosition(self.ship_pointer, x, y)
end

function ship_:WaitFor(time_to_wait)
    local end_time = self.time + time_to_wait
    while self.time < end_time do
        coroutine.yield()
    end
end

function ship_:WaitForAnd(time_to_wait, dothis)
    local end_time = self.time + time_to_wait
    while self.time < end_time do
        dothis()
        coroutine.yield()
    end
end

function ship_:FleeFor(time_to_flee)
    local end_time = self.time + time_to_flee
    while self.time < end_time do
        if self.target.valid then
            local dp = (self.target.position - self.position):normalise()
            local right_vector = Vector:new(math.cos(math.rad(self.angle)), -math.sin(math.rad(self.angle)))
            local dotp = dp:dot(right_vector)
            
            self:SetAll(-dp.x, -dp.y, -dotp, false)
        else
            self:PickClosestTarget()
        end
        coroutine.yield()
    end
end

function ship_:FleeForAnd(time_to_flee, dothis)
    local end_time = self.time + time_to_flee
    while self.time < end_time do
        if self.target.valid then
            local dp = (self.target.position - self.position):normalise()
            local right_vector = Vector:new(math.cos(math.rad(self.angle)), -math.sin(math.rad(self.angle)))
            local dotp = dp:dot(right_vector)
            
            self:SetAll(-dp.x, -dp.y, -dotp, false)
        else
            self:PickClosestTarget()
        end
        dothis()
        coroutine.yield()
    end
end

function ship_:AttackFor(time_to_attack, min_range, max_range)
    local end_time = self.time + time_to_attack
    while self.time < end_time do
        if self.target.valid == true then
            local range = (self.target.position - self.position):length()
            local dp = (self.target.position - self.position):normalise()
            local right_vector = Vector:new(math.cos(math.rad(self.angle)), -math.sin(math.rad(self.angle)))
            local dotp = dp:dot(right_vector)
            local firing = math.abs(dotp) < 0.4 and range < 1000

            if range < min_range then
                --Back off
                self:SetAll(-dp.x, -dp.y, dotp, firing)
            elseif range > max_range then
                --Advance
                self:SetAll(dp.x, dp.y, dotp, firing)
            else
                --Strafe
                self:SetAll(dp.y, -dp.x, dotp, firing)
            end
        else
            self:PickClosestTarget()
        end
        coroutine.yield()
    end
end

function ship_:AttackForAnd(time_to_attack, min_range, max_range, dothis)
    local end_time = self.time + time_to_attack
    while self.time < end_time do
        if self.target.valid == true then
            local range = (self.target.position - self.position):length()
            local dp = (self.target.position - self.position):normalise()
            local right_vector = Vector:new(math.cos(math.rad(self.angle)), -math.sin(math.rad(self.angle)))
            local dotp = dp:dot(right_vector)
            local firing = math.abs(dotp) < 0.4 and range < 1000

            if range < min_range then
                --Back off
                self:SetAll(-dp.x, -dp.y, dotp, firing)
            elseif range > max_range then
                --Advance
                self:SetAll(dp.x, dp.y, dotp, firing)
            else
                --Strafe
                self:SetAll(dp.y, -dp.x, dotp, firing)
            end
        else
            self:PickClosestTarget()
        end
        dothis()
        coroutine.yield()
    end
end

local ship_mt = 
{
	__newindex = function (t, n, v)
		print ("Unable to change contents of ship")
	end
}

setmetatable(ship_, ship_mt)

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
	ship = ship_,
}

local env_mt = {__index = env_cage}
local env = {}
setmetatable(env, env_mt)

--env is now ready, needs to be set as environment for the AI to be run in.
--Then a coroutine must be created.
return env

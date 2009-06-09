--Section class
require "Vector"
require "Deepcopy"

Section = 
{
	challenge_pointer = 0,
	section_type = "",
	position = Vector:new(0, 0),
	angle = 0,
	health = {override = false, value = 0},
	subsections = {},
	section_id = -1,
	parent = nil,
	child_id = 0,
	child_count = 0,
	edit_mode = {edit_existing = false, ship_id = -1},
	action = "None",
}

function Section:New(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	self.action = "Create"
	return o
end

function Section:OverrideHealth(health)
	self.health.override = true
	self.health.value = health
end

function Section:AttachChild(child)
	self.child_count = self.child_count + 1
	self.subsections[self.child_count] = child
	child.parent = self
	child.child_id = #self.subsections
end

function Section:Detach()
	if self.parent ~= nil then
		--self.parent.subsection[self.child_id] = nil
		--self.parent = nil
		self.action = "Delete"
	else
		error("Cannot detach, not currently attached")
	end

	copy = deepcopy(self)
	copy.parent = nil
	copy.action = "None"

	return copy
end

--Spawns the ship prototype into the world
function Section:Spawn()
	
end

--Updates an existing ship with the new prototype
function Section:Update()
	if self.edit_mode.edit_existing == true and self.edit_mode.ship_id ~= -1 then
		
	else
		error("Cannot call UpdateCore on a new ship, can only call it on a ship obtained through GetCore(ship_id)")
	end
end

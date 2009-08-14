--Section class
require "Vector"
require "Deepcopy"

Section = 
{
	section_type = "",
	angle = 0,
	section_id = -1,
	parent = nil,
	child_id = 0,
	child_count = 0,
	action = "None",
}

function Section:new(o)
	o = o or {}
	if o.position == nil then
		o.position = Vector:new(0, 0)
	end
	if o.subsections == nil then
		o.subsections = {}
	end
	if o.health == nil then
		o.health = {override = false, value = 0}
	end
	if o.edit_mode == nil then
		o.edit_mode = {edit_existing = false, ship_id = -1}
	end
	setmetatable(o, self)
	self.__index = self
	self.action = "Create"
	return o
end

function Section:overrideHealth(health)
	self.health.override = true
	self.health.value = health
end

function Section:attachChild(child)
	self.child_count = self.child_count + 1
	self.subsections[self.child_count] = child
	child.parent = self
	child.child_id = #self.subsections
end

function Section:detach()
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


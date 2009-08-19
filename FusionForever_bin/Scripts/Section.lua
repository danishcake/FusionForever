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
	edit_mode = {edit_existing = false, ship_id = -1},
	action = "None",
}

Section.__index = Section

function Section:new(o)
	o = o or {}
	if o.position == nil then
		o.position = {x = 0, y = 0}
	end
	if o.subsections == nil then
		o.subsections = {}
	end
	if o.health == nil then
		o.health = {override = false, value = 0}
	end
	if o.action == nil then
		o.action = "Create"
	end
	
	setmetatable(o, self)
	return o
end

function Section:overrideHealth(health)
	self.health.override = true
	self.health.value = health
end

function Section:setAction(action)
	for i,v in ipairs(self.subsections) do
		v.setAction(action)
	end
	self.action = action
end

function Section:attachChild(child)
	self.child_count = self.child_count + 1
	self.subsections[self.child_count] = child
	child.parent = self
	child.child_id = self.child_count
	child:setAction("Create")
end

function Section:detach()
	if self.parent ~= nil then
		self.action = "Delete"
	else
		error("Cannot detach, not currently attached")
	end

	copy = deepcopy(self)
	copy.parent = nil
	copy.action = "None"

	return copy
end

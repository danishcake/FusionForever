--Provides an api to assemble a ship from a challenge script
require "Section"

builder_ =
{
	challenge_pointer = 0
}

function builder_:LateInit(_challenge_pointer)
	self.challenge_pointer = _challenge_pointer
end

--Creates a new ship with just a core of the specified type
function builder_:CreateNew(core_type)
	local core = Section:New{section_type = core_type, challenge_pointer = self.challenge_pointer}
	return core
end

--Obtains a core representing the specified ship
function builder_:GetCore(ship_id)
	--Call into C to get a populated core object back
	local core = GetCore(ship_id)
	core.challenge_pointer = self.challenge_pointer
	core.edit_mode.existing = true
	core.edit_mode.ship_id = ship_id
	return core
end


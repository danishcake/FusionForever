--Provides an api to assemble a ship from a challenge script
require "Section"

builder_ =
{
	challenge_pointer = 0
}

function builder_:init(_challenge_pointer)
	self.challenge_pointer = _challenge_pointer
end

--Creates a new ship with just a core of the specified type
function builder_:createNew(core_type)
	return Section:new{section_type = core_type}
end

--Obtains a core representing the specified ship
function builder_:getDesign(ship_id)
	--Call into C to get a populated core object back
	local core = GetDesign(self.challenge_pointer, ship_id)
	core.challenge_pointer = self.challenge_pointer
	core.edit_mode = {}
	core.edit_mode.edit_existing = true
	core.edit_mode.ship_id = ship_id
	core = Section:new(core)
	return core
end

--Spawns the ship prototype into the world
function builder_:spawnDesign(core, position, angle, force, health_scale)
	return SpawnDesign(self.challenge_pointer, core, position, angle, force, health_scale)
end

--Updates an existing ship with the new prototype
function builder_:updateShip(core)
	if core.edit_mode.edit_existing == true and core.edit_mode.ship_id ~= -1 then
		UpdateDesign(self.challenge_pointer, core)
		coroutine.yield()
	else
		error("Cannot call UpdateCore on a new ship, can only call it on a ship obtained through GetCore(ship_id)")
	end
end

function builder_:getSectionMetadata(section)
	return GetSectionMetadataByType(self.challenge_pointer, section.section_type)
end

function builder_:getSectionTypeMetadata(section_type)
	return GetSectionMetadataByType(self.challenge_pointer, section_type)
end
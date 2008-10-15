if ship.target.valid == false then
	ship.PickRandomTarget()
end

--Flee!
local end_time = ship.time + 10
while ship.time < end_time do
	if ship.target.valid == true then
		local range = math.sqrt ((ship.target.position.x - ship.position.x) * (ship.target.position.x - ship.position.x) + (ship.target.position.y - ship.position.y) * (ship.target.position.y - ship.position.y))
		local dx = (ship.target.position.x - ship.position.x) / range
		local dy = (ship.target.position.y - ship.position.y) / range
		local right_vector_dx = math.cos(ship.angle)
		local right_vector_dy = -math.sin(ship.angle)
		local dotp = dx * right_vector_dx + dy * right_vector_dy
		ship.SetAll(-dx, -dy, -dotp, false)
	end
	coroutine.yield()
end

--Attack
local end_time = ship.time + 12
while ship.time < end_time do
	if ship.target.valid == true then
		local range = math.sqrt ((ship.target.position.x - ship.position.x) * (ship.target.position.x - ship.position.x) + (ship.target.position.y - ship.position.y) * (ship.target.position.y - ship.position.y))
		local dx = (ship.target.position.x - ship.position.x) / range
		local dy = (ship.target.position.y - ship.position.y) / range
		local right_vector_dx = math.cos(ship.angle)
		local right_vector_dy = -math.sin(ship.angle)
		local dotp = dx * right_vector_dx + dy * right_vector_dy
		local firing = math.abs(dotp) < 0.4

		if range < 100 then
			--Back off
			ship.SetAll(-dx, -dy, dotp, firing)
		elseif range > 500 then
			--Advance
			ship.SetAll(dx, dy, dotp, firing)
		else
			--Strafe
			ship.SetAll(dy, -dx, dotp, firing)
		end
	end
	coroutine.yield()
end





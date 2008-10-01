if ship.position.x < -20 then
	ship.SetMoveDirection(0.1, 0)
elseif ship.position.x > 20 then
	ship.SetMoveDirection(-0.1, 0)
end

coroutine.yield()

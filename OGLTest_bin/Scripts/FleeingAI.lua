if ship.target.valid == false then
	ship.PickRandomTarget()
end

--Flee!

ship.FleeFor(10)

--Attack
ship.AttackFor(13, 100, 500)





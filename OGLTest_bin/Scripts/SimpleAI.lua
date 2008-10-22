if ship.target.valid == false then
	ship.PickRandomTarget()
end

--Flee!
ship.FleeFor(2)

--Attack
ship.AttackFor(10, 100, 500)
ship.AttackFor(10, 200, 300)
ship.AttackFor(10, 1000, 1200)





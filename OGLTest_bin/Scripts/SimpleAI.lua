if ship.target.valid == false then
	ship.PickRandomTarget()
end

camera_x = ship.position.x
camera_y = ship.position.y

--Flee!
local update_camera = function()
	local x
	local y
	if ship.target.valid then
		x = (ship.position.x + ship.target.position.x) / 2
		y = (ship.position.y + ship.target.position.y) / 2
	else
		x = ship.position.x
		y = ship.position.y
	end
	camera_x = camera_x + (x - camera_x) * 0.1
	camera_y = camera_y + (y - camera_y) * 0.1
	if camera_x - ship.position.x > 500 then 
		camera_x = ship.position.x + 500 end
	if camera_x - ship.position.x < -500 then 
		camera_x = ship.position.x - 500 end
	if camera_y - ship.position.y > 500 then 
		camera_y = ship.position.y + 500 end
	if camera_y - ship.position.y < -500 then 
		camera_y = ship.position.y - 500 end
	ship.SetCameraPosition(camera_x, camera_y)
end

--Attack
ship.AttackForAnd(10, 100, 500, update_camera)
ship.AttackForAnd(10, 200, 300, update_camera)
ship.AttackForAnd(10, 1000, 1200, update_camera)





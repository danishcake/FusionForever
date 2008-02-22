require "Utility"

Challenge = {

Script = coroutine.create(function()
LoadShip("LuaShip2.lua")
SetAI("KeyboardAI")
AddAsFriend()

for z = 1, 3 do
	for x = 1, 8 do
		for y = 1, 8 do
		  LoadShip("LuaShip2.lua")
		  SetAI("RotatingAI", 0.2)
		  SetColor(0,128,0)
		  SetPosition(x * 50, y * 50)
		  ScaleHealth(0.01)
		  AddAsEnemy()
		  Predicate.WaitFor(0.1)
		end
	end
	Predicate.WaitTillDead()
end
Predicate.WaitTillDead()

LoadShip("SpaceStation.lua")
SetAI("RotatingAI", 0.05)
SetColor(0,128,128)
SetPosition(300,0)
AddAsEnemy()


end),
EntryPoint = function()
if coroutine.status(Challenge.Script) ~= "dead" then
	coroutine.resume(Challenge.Script)
	return false	--not over yet
else
	return true		--Over now
end
end
}
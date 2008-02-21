Challenge = {
WaitTillDead = function()
  coroutine.yield() -- I would ideally like to reduce this to one command wait_till_enemy_dead
  while ENEMY_COUNT > 0 do
    coroutine.yield()
  end
end,

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
		  ScaleHealth(0.0001)
		  AddAsEnemy()
		end
	end
	Challenge.WaitTillDead()
end
Challenge.WaitTillDead()
LoadShip("LuaShip2.lua")
SetAI("RotatingAI", -0.2)
SetColor(0,255,0)
SetPosition(50, 20)
ScaleHealth(5)
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
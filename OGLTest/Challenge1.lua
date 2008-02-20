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

for i = 0, 10 do
  LoadShip("LuaShip2.lua")
  SetAI("RotatingAI", 0.2)
  SetColor(0,255,0)
  SetPosition(i * 20, 0)
  AddAsEnemy()
  Challenge.WaitTillDead()
end
LoadShip("LuaShip2.lua")
SetAI("RotatingAI", -0.2)
SetColor(0,255,0)
SetPosition(50, 20)
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
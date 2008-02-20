Challenge = {
Script = coroutine.create(function()
LoadShip("LuaShip2.lua")
SetAI("KeyboardAI")
AddAsFriend()
for i = 0
LoadShip("LuaShip2.lua")
SetAI("RotatingAI", 0.2)
SetColor(0,255,0)
AddAsEnemy()

coroutine.yield() -- I would ideally like to reduce this to one command wait_till_enemy_dead
while ENEMY_COUNT > 0 do
	coroutine.yield()
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
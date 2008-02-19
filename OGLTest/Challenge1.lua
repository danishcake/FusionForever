Challenge = {
Script = coroutine.create(function()
AddFriend("LuaShip2.lua")
AddEnemy("LuaShip2.lua")
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
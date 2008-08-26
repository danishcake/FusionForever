--Utility.lua
--Contains various scripting utilities for flow control. 
--When called these routines will not return until their conditions are fulfilled.

Predicate = {
WaitTillEnemiesDead = function(force)
  coroutine.yield()
  while ENEMY_COUNT[force] > 0 do
    coroutine.yield()
  end
end,
WaitTillEnemyCountLessThan = function(force, count)
	coroutine.yield()
	while ENEMY_COUNT[force] > count do
		coroutine.yield()
	end
end,
WaitTillFriendCountLessThan = function(force, count)
	coroutine.yield()
	LogError(FRIEND_COUNT[force])
	while FRIEND_COUNT[force] > count do
		coroutine.yield()
	end
end,
WaitTillForceCountLessThan = function(force, count)
	coroutine.yield()
	while FORCE_COUNT[force] > count do
		coroutine.yield()
	end
end,
WaitTillFriendsDead = function(force)
  coroutine.yield()
  while FRIEND_COUNT[force] > 0 do
    coroutine.yield()
  end
end,
WaitFor = function(time)
  local end_time = time + TOTAL_TIME
  while TOTAL_TIME < end_time do
    coroutine.yield()
  end
end,
WaitTillShipDead = function(ship_id)
  coroutine.yield()
  while IsAlive(ship_id) do
    Predicate.WaitFor(0.25)
    coroutine.yield()
  end  
end,
}

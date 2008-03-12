--Utility.lua
--Contains various scripting utilities for flow control. 
--When called these routines will not return until their conditions are fulfilled.

Predicate = {
WaitTillEnemiesDead = function()
  coroutine.yield()
  while ENEMY_COUNT > 0 do
    coroutine.yield()
  end
end,
WaitTillFriendsDead = function()
  coroutine.yield()
  while FRIEND_COUNT > 0 do
    coroutine.yield()
  end
end,
WaitFor = function(time)
  local end_time = time + TOTAL_TIME
  while TOTAL_TIME < end_time do
    coroutine.yield()
  end
end
}
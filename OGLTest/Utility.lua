Predicate = {
WaitTillDead = function()
  coroutine.yield() -- I would ideally like to reduce this to one command wait_till_enemy_dead
  while ENEMY_COUNT > 0 do
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
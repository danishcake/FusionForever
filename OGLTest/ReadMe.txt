The game is base around challenges - the challenges are loaded and then must be completed.

A challenge consist of a script that will load a series of ships, set their AI, colors, etc, then wait for progress before continuing. When the script is complete the challenge is won

A challenge has two parts:
a) The EntryPoint
b) The Script

Challenge = {
Script = coroutine.create(function()
--Script actions in here
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

I may add multiple scripts and win conditions in future.
--Challenge1.lua
--This loads a challenge of three waves of 64 extremely weak enemies
--followed by a single 'Space Station' enemy which is hard to kill
--without using it's weakspot

--This ensures the Predicate table is loaded, providing 
--Predicate.WaitFor(x)          Waits for x seconds
--Predicate.WaitTillDead()      Waits for all enemies to die
require "Utility"

--This is the Challenge table. It must contain the function EntryPoint to be valid.
--EntryPoint is called by the simulation every frame until it returns true.
--The intention is for it to resume coroutine(s).

Challenge = {
Script = coroutine.create(function()
LoadShip("LuaShip2.lua")				--Loads a ship but doesn't add it to the world
SetAI("KeyboardAI")					--Sets the last loaded ships AI
AddAsFriend()						--Adds the ship as a friend

for i = 1, 5 do					--in batches of 64 that must be killed entirely
	LoadShip("MissilePlatform.lua")				--Loads a ship but doesn't add it to the world
	SetAI("RotatingAI", -0.1)					--Sets the last loaded ships AI
	SetPosition(50*i,400)
	AddAsFriend()						--Adds the ship as a friend
	Predicate.WaitFor(0.2)
end

Predicate.WaitFor(1)

ship_id = LoadShip("SpikeyShip.lua")
SetAI("RotatingAI", 0.3)
SetColor(0,128,128)
SetPosition(0,0)
AddAsEnemy()

Predicate.WaitTillShipDead(ship_id)
Predicate.WaitFor(1)

LoadShip("SpaceStation.lua")
SetAI("RotatingAI", 0.05)
SetColor(0,128,128)
SetPosition(150,-150)
AddAsEnemy()
Predicate.WaitFor(1)

LoadShip("SpaceStation.lua")
SetAI("RotatingAI", -0.05)
SetColor(0,128,128)
SetPosition(150,150)
AddAsEnemy()
Predicate.WaitFor(1)

LoadShip("SpaceStation.lua")
SetAI("RotatingAI", 0.05)
SetColor(0,128,128)
SetPosition(-150,150)
AddAsEnemy()
Predicate.WaitFor(1)

LoadShip("SpaceStation.lua")
SetAI("RotatingAI", -0.05)
SetColor(0,128,128)
SetPosition(-150,-150)
AddAsEnemy()

Predicate.WaitTillEnemiesDead()

							--The following loops load 3x64 enemies
for z = 1, 3 do						--with a 0.1s gap between each enemy and
	for x = 1, 4 do					--in batches of 64 that must be killed entirely
		for y = 1, 4 do
		  LoadShip("LuaShip2.lua")
		  SetAI("RotatingAI", 0.02)		--Sets the AI to rotate at 20% speed
		  SetColor(0,128,0)
		  SetPosition(x * 50, y * 50)
		  --ScaleHealth(0.01)			--Makes the ship very weak
		  AddAsEnemy()				--Adds to the enemy team
		  Predicate.WaitFor(0.1)
		end
	end
	Predicate.WaitTillEnemiesDead()
end

end),							--End of resumable coroutine
							--Now the coroutine status is "dead"

EntryPoint = function()
	if coroutine.status(Challenge.Script) ~= "dead" then
		local run_ok, error_message = coroutine.resume(Challenge.Script)
		if run_ok then
			return false	--Script still executing
		else
			return error_message
		end
	else
		LogError("Lua: Script over")
		return true	--Script finished
	end
end
}
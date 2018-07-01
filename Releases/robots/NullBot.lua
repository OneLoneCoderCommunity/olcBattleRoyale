require "olcBRE"


-- Name your bot
function IdentifyYourself()
	return "nullbot"
end


-- Equivalent of main loop
function Update()
	
	-- Check Walls
 	n, e, s, w = GetWalls()
	if n < 20.0 then
		if math.random() >= 0.5 then
			TurnRight()
			MoveBackward()
			TurnRight()
		else
			TurnLeft()
			MoveBackward()
			TurnLeft()
		end
	else
		MoveForward()
	end


	me = GetStatus()
	if me.health < 3 then
	--	SelfDestruct()
	end
end
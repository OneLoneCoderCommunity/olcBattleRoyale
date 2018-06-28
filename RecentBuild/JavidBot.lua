require "olcBRE"

-- Name your bot
function IdentifyYourself()
	return "javidx9"
end


-- Equivalent of main loop
function Update()
	-- Check Walls
 	n, e, s, w = GetWalls()
	if n < 20.0 then		
		TurnRight()
		MoveBackward()
		TurnRight()
	else
		MoveForward()
	end
	
	-- Check Enemy Locality
 	n, e, s, w = GetRadar()
	if n < 20.0 then
		ActivateShield()
	end

	if s < 20.0 then
		ActivateShield()
	end

	map = GetMap()	
	me = GetStatus()
	if me.health <= 1 then
		SelfDestruct()
	end


	TurnToAngle(math.atan2(map[1].y-me.y, map[1].x-me.x))

	if math.random() >= 0.5 then
		MoveForward()
		TurnRight()
		MoveForward()
		Fire()
	else
		MoveForward()
		Fire()
	end	
end
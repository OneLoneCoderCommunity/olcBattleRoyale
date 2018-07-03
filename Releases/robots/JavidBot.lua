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
	if n < 40.0 then
		Shield(ON)
	else
		Shield(OFF)
	end

	map = GetMap()	
	me = GetStatus()
	battle = GetBattle()

	-- Count how many opponents were visible
	count = 0
	for _ in pairs(map) do count = count + 1 end
	
	-- Find first opponent not on my team
	i = 0
	while i < count and me.team == map[i].team do
		i = i + 1
	end 

	if battle.enemies > 0 then
		TurnToAngle(math.atan2(map[i].y-me.y, map[i].x-me.x))
		Fire()
	else
		while true do
			TurnLeft()
		end
	end
	

end
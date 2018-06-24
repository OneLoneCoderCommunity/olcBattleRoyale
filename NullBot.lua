local CMD_NOTHING	= 0
local CMD_FORWARDS	= 1
local CMD_BACKWARDS	= 2
local CMD_TURNLEFT	= 3
local CMD_TURNRIGHT	= 4
local CMD_FIRE		= 5
local CMD_SHIELD	= 6
local CMD_MINE		= 7
local CMD_DESTROY	= 8

local RobotGUID = 0

function EstablishLink(ID)
	RobotGUID = ID
end

function MoveForward()
	_PerformAction(RobotGUID, CMD_FORWARDS)
end

function MoveBackward()
	_PerformAction(RobotGUID, CMD_BACKWARDS)
end

function TurnLeft()
	_PerformAction(RobotGUID, CMD_TURNLEFT)
end

function TurnRight()
	_PerformAction(RobotGUID, CMD_TURNRIGHT)
end

function ActivateShield()
	_PerformAction(RobotGUID, CMD_SHIELD)
end

function GetWalls()
	return _ReadSensors(RobotGUID, 0)
end

function GetRadar()
	return _ReadSensors(RobotGUID, 1)
end

-- All above here will be hidden to keep script simple

-- Name your bot
function IdentifyYourself()
	return "nullbot"
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
end
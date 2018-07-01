local CMD_NOTHING = 0
local CMD_FORWARDS = 1
local CMD_BACKWARDS = 2
local CMD_TURNLEFT = 3
local CMD_TURNRIGHT = 4
local CMD_FIRE = 5
local CMD_SHIELD_ON = 6
local CMD_SHIELD_OFF = 7
local CMD_CLOAK_ON = 8
local CMD_CLOAK_OFF = 9
local CMD_MINE = 10
local CMD_DESTROY = 11
local CMD_TURNANGLE = 12	

ON = 1
OFF = 0

local RobotGUID = 0

function EstablishLink(ID)
	RobotGUID = ID
end

function Fire()
	_PerformAction(RobotGUID, CMD_FIRE)
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

function TurnToAngle(a)
	_PerformAction(RobotGUID, CMD_TURNANGLE, a)
end

function Shield(a)
	if a == ON then
		_PerformAction(RobotGUID, CMD_SHIELD_ON)
	else
		_PerformAction(RobotGUID, CMD_SHIELD_OFF)
	end
end

function Cloak(a)
	if a == ON then
		_PerformAction(RobotGUID, CMD_CLOAK_ON)
	else
		_PerformAction(RobotGUID, CMD_CLOAK_OFF)
	end
end

function SelfDestruct()
	_PerformAction(RobotGUID, CMD_DESTROY)
end






function GetWalls()
	return _ReadSensors(RobotGUID, 0)
end

function GetRadar()
	return _ReadSensors(RobotGUID, 1)
end

function GetMap()
	return _ReadSensors(RobotGUID, 2)
end

function GetStatus()
	return _ReadSensors(RobotGUID, 3)
end

function GetBattle()
	return _ReadSensors(RobotGUID, 4)
end



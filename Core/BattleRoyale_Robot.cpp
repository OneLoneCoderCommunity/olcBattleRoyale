/*
OneLoneCoder.com - Battle Royale Engine - Console User Interface

License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.

GNU GPLv3
~~~~~~~~~
https://github.com/OneLoneCoder/videos/blob/master/LICENSE

Author of Original Works
~~~~~~~~~~~~~~~~~~~~~~~~
Twitter:	http://twitter.com/javidx9
Blog:		http://www.onelonecoder.com
YouTube:	http://www.youtube.com/javidx9
Github:		http://www.github.com/OneLoneCoder/videos
Discord:	https://discord.gg/WhwHUMV
Twitch:		http://www.twitch.tv/javidx9
SoundCloud: https://www.soundcloud.com/onelonecoder
*/

#include "BattleRoyale_Robot.h"
#include "BattleRoyale_Engine.h"

cRobot::cRobot()
{

}

cRobot::cRobot(const cRobot& r)
{
	nCurrentState = r.nCurrentState;
	nNextState = r.nNextState;
	status = r.status;
	wall = r.wall;
	enemy = r.enemy;
	nCommandToExecute = r.nCommandToExecute;
	fCommandTimeRemaining = r.fCommandTimeRemaining;
	map = r.map;
	bPerformingAction = r.bPerformingAction.load();
	battle = r.battle;
}



std::string cRobot::Test(std::string sFile)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	lua_register(L, "_PerformAction", lua_PerformAction);
	lua_register(L, "_ReadSensors", lua_ReadSensors);

	int r = luaL_loadfile(L, sFile.c_str());
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		lua_close(L);
		return errormsg;
	}

	int i = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (i != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		lua_close(L);
		return errormsg;
	}

	return "OK";
}

void cRobot::Start()
{
	// Start the robots running thread, with a random delay of up to 1 second
	nNextState = nCurrentState = STATE_WAIT_FOR_COMMAND;
	threadLuaScript = std::thread(&cRobot::LuaScript, this, status.script, rand() % 1000);
}

void cRobot::LuaScript(std::string sFile, int start_delay)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	lua_register(L, "_PerformAction", lua_PerformAction);
	lua_register(L, "_ReadSensors", lua_ReadSensors);


	int r = luaL_loadfile(L, sFile.c_str());
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		return;
	}

	int i = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (i != LUA_OK)
	{
		return;
	}


	lua_getglobal(L, "IdentifyYourself");
	if (lua_isfunction(L, -1))
	{
		if (lua_pcall(L, 0, 1, 0) == LUA_OK)
		{
			status.name = lua_tostring(L, -1);
		}
	}

	lua_getglobal(L, "EstablishLink");
	if (lua_isfunction(L, -1))
	{
		lua_pushlightuserdata(L, this);
		if (lua_pcall(L, 1, 0, 0) == LUA_OK)
		{

		}
	}

	srand(clock());
	std::this_thread::sleep_for(std::chrono::milliseconds(start_delay));

	while (!status.malfunction)
	{
		lua_getglobal(L, "Update");
		if (lua_isfunction(L, -1))
		{
			if (lua_pcall(L, 0, 0, 0) == LUA_OK)
			{
				// Robot is good
			}
			else
			{
				// Robot has runtime error, so malfunction
				status.malfunction = true;
				status.sDebugOutput = lua_tostring(L, -1);
			}
		}
		else
		{

		}

		lua_settop(L, 0);
		//this_thread::sleep_for(chrono::milliseconds(10));
	}
}

void cRobot::UpdateStateMachine(float fElapsedTime, OneLoneCoder_BattleRoyale* pHost)
{
	if (status.cloaked)
		status.energy -= fElapsedTime;

	if (status.shielded)
		status.energy -= fElapsedTime;

	if (status.energy <= 0.0f)
	{
		status.energy = 0.0f;
		status.shielded = false;
		status.cloaked = false;
	}

	if (status.cooldown > 0.0f)
		status.cooldown -= fElapsedTime;

	status.angle += 3.14159f * 2.0f;
	status.angle = fmod(status.angle, 3.14159f * 2.0f);

	// Calculate residual time this update
	float fResidualTime = fElapsedTime;
	fCommandTimeRemaining -= fElapsedTime;
	if (fCommandTimeRemaining < 0.0f)
	{
		fResidualTime = fCommandTimeRemaining + fElapsedTime;
		fCommandTimeRemaining = 0.0f;
	}

	// Robot State Machine
	switch (nCurrentState)
	{
	case STATE_WAIT_FOR_COMMAND: // Prepare action states based on command
		switch (nCommandToExecute)
		{
		case CMD_NOTHING: // Do Nothing
			break;

		case CMD_FORWARDS:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = BattleRoyale_Parameters::fRobotMoveTime;
			nNextState = STATE_MOVING_FORWARDS;
			break;

		case CMD_BACKWARDS:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = BattleRoyale_Parameters::fRobotMoveTime;
			nNextState = STATE_MOVING_BACKWARDS;
			break;

		case CMD_TURNLEFT:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = BattleRoyale_Parameters::fRobotTurnTime;
			nNextState = STATE_TURNING_LEFT;
			break;

		case CMD_TURNRIGHT:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = BattleRoyale_Parameters::fRobotTurnTime;
			nNextState = STATE_TURNING_RIGHT;
			break;

		case CMD_FIRE:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = 0.1f;
			if (status.cooldown <= 0.0f)
			{
				pHost->AddBullet(status.posx, status.posy, cosf(status.angle), sinf(status.angle), status.id);
				status.cooldown = BattleRoyale_Parameters::fGunCooldown;
			}
			nNextState = STATE_COMMAND_COMPLETE;
			break;

		case CMD_DESTROY:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = 0.1f;
			for (int i = 0; i < BattleRoyale_Parameters::nSelfDestructBullets; i++)
			{
				float a = (i / (float)BattleRoyale_Parameters::nSelfDestructBullets) * 3.14159f * 2.0f;
				pHost->AddBullet(status.posx, status.posy, cosf(a), sinf(a), status.id);
			}
			status.health = 0;
			nNextState = STATE_COMMAND_COMPLETE;
			break;

		case CMD_TURNANGLE:
		{
			nCommandToExecute = CMD_NOTHING;
			fArgument += 3.14159f * 2.0f;
			fArgument = fmod(fArgument, 3.14159f * 2.0f);

			float fAngleDiff = fArgument - status.angle;

			fAngleDiff += 3.14159f * 2.0f;
			fAngleDiff = fmod(fAngleDiff, 3.14159f * 2.0f);

			if (fAngleDiff < 3.14159f)
			{
				fCommandTimeRemaining = fabs((fAngleDiff) / status.fTurnSpeed);
				nNextState = STATE_TURNING_RIGHT;
			}
			else
			{
				fCommandTimeRemaining = fabs(((2.0f * 3.14159f) - fAngleDiff) / status.fTurnSpeed);
				nNextState = STATE_TURNING_LEFT;
			}
		}
		break;

		case CMD_SHIELD_ON:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = 0.1f;
			if (status.energy >= 0.0f)
			{
				status.shielded = true;
			}
			nNextState = STATE_COMMAND_COMPLETE;
			break;

		case CMD_SHIELD_OFF:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = 0.1f;
			status.shielded = false;
			nNextState = STATE_COMMAND_COMPLETE;
			break;

		case CMD_CLOAK_ON:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = 0.1f;
			if (status.energy >= 0.0f)
			{
				status.cloaked = true;
			}
			nNextState = STATE_COMMAND_COMPLETE;
			break;

		case CMD_CLOAK_OFF:
			nCommandToExecute = CMD_NOTHING;
			fCommandTimeRemaining = 0.1f;
			status.cloaked = false;			
			nNextState = STATE_COMMAND_COMPLETE;
			break;
		}
		break;

	case STATE_MOVING_FORWARDS:
		status.posx += status.fMoveSpeed * cosf(status.angle) * fResidualTime;
		status.posy += status.fMoveSpeed * sinf(status.angle) * fResidualTime;
		if (fCommandTimeRemaining == 0.0f)
			nNextState = STATE_COMMAND_COMPLETE;
		break;

	case STATE_MOVING_BACKWARDS:
		status.posx -= status.fMoveSpeed * cosf(status.angle) * fResidualTime;
		status.posy -= status.fMoveSpeed * sinf(status.angle) * fResidualTime;
		if (fCommandTimeRemaining == 0.0f)
			nNextState = STATE_COMMAND_COMPLETE;
		break;

	case STATE_TURNING_LEFT:
		status.angle -= status.fTurnSpeed * fResidualTime;
		if (fCommandTimeRemaining == 0.0f)
			nNextState = STATE_COMMAND_COMPLETE;
		break;

	case STATE_TURNING_RIGHT:
		status.angle += status.fTurnSpeed * fResidualTime;
		if (fCommandTimeRemaining == 0.0f)
			nNextState = STATE_COMMAND_COMPLETE;
		break;



	case STATE_COMMAND_COMPLETE: // Current command has finished, so signal PerformAction() to continue
		nNextState = STATE_WAIT_FOR_COMMAND;
		nCommandToExecute = CMD_NOTHING;
		bPerformingAction = false;
		std::unique_lock<std::mutex> lm(muxExecutingCommand);
		cvExecutingCommand.notify_one();
		break;


	}

	nCurrentState = nNextState;
}

int cRobot::lua_PerformAction(lua_State *L)
{
	// Get pointer to this class
	cRobot* robot = (cRobot*)lua_touserdata(L, 1);

	// Get Command identifier
	int nCommand = (int)lua_tointeger(L, 2);

	// Get optional argument 1
	float fArg1 = (float)lua_tonumber(L, 3);

	// Call appropriate funtion
	robot->fArgument = fArg1;
	robot->nCommandToExecute = (COMMANDS)nCommand;

	// Block thread until command complete signal
	std::unique_lock<std::mutex> lm(robot->muxExecutingCommand);
	robot->bPerformingAction = true;
	while (robot->bPerformingAction)
		robot->cvExecutingCommand.wait(lm);

	// No arguments to return
	return 0;
}

int cRobot::lua_ReadSensors(lua_State *L)
{
	// Get pointer to this class
	cRobot* robot = (cRobot*)lua_touserdata(L, 1);

	// Get Command identifier
	int nSensorBank = (int)lua_tointeger(L, 2);

	if (nSensorBank == 0) // Wall
	{
		std::unique_lock<std::mutex> lm(robot->muxUpdatingSensors);
		lua_pushnumber(L, robot->wall.n);
		lua_pushnumber(L, robot->wall.e);
		lua_pushnumber(L, robot->wall.s);
		lua_pushnumber(L, robot->wall.w);
		return 4;
	}

	if (nSensorBank == 1) // Enemy
	{
		std::unique_lock<std::mutex> lm(robot->muxUpdatingSensors);
		lua_pushnumber(L, robot->enemy.n);
		lua_pushnumber(L, robot->enemy.e);
		lua_pushnumber(L, robot->enemy.s);
		lua_pushnumber(L, robot->enemy.w);
		return 4;
	}

	if (nSensorBank == 2) // Map
	{
		std::unique_lock<std::mutex> lm(robot->muxUpdatingSensors);

		// Create table with number of robots as entries
		lua_createtable(L, robot->map.entries.size(), 0);

		int nRobotCount = 0;
		for (auto &m : robot->map.entries)
		{
			// Push index
			lua_pushinteger(L, nRobotCount);

			// Push properties table {name, x, y}
			lua_createtable(L, 0, 4);

			lua_pushnumber(L, m.id);
			lua_setfield(L, -2, "id");

			lua_pushnumber(L, m.team);
			lua_setfield(L, -2, "team");

			lua_pushnumber(L, m.x);
			lua_setfield(L, -2, "x");

			lua_pushnumber(L, m.y);
			lua_setfield(L, -2, "y");

			lua_settable(L, -3);

			nRobotCount++;

		}

		return 1;
	}

	if (nSensorBank == 3) // Self
	{
		std::unique_lock<std::mutex> lm(robot->muxUpdatingSensors);

		// Create table with number of robots as entries
		lua_createtable(L, 0, 9);

		lua_pushnumber(L, robot->status.id);
		lua_setfield(L, -2, "id");

		lua_pushnumber(L, robot->status.team);
		lua_setfield(L, -2, "team");

		lua_pushnumber(L, robot->status.posx);
		lua_setfield(L, -2, "x");

		lua_pushnumber(L, robot->status.posy);
		lua_setfield(L, -2, "y");

		lua_pushnumber(L, robot->status.angle);
		lua_setfield(L, -2, "angle");

		lua_pushnumber(L, robot->status.health);
		lua_setfield(L, -2, "health");

		lua_pushboolean(L, robot->status.shielded);
		lua_setfield(L, -2, "shielded");

		lua_pushboolean(L, robot->status.cloaked);
		lua_setfield(L, -2, "cloaked");

		lua_pushnumber(L, robot->status.energy);
		lua_setfield(L, -2, "energy");

		return 1;
	}

	if (nSensorBank == 4) // Game
	{
		std::unique_lock<std::mutex> lm(robot->muxUpdatingSensors);

		// Create table with number of robots as entries
		lua_createtable(L, 0, 3);

		lua_pushnumber(L, robot->battle.gametime);
		lua_setfield(L, -2, "time");

		lua_pushnumber(L, robot->battle.enemies);
		lua_setfield(L, -2, "enemies");

		lua_pushnumber(L, robot->battle.allies);
		lua_setfield(L, -2, "allies");

		lua_pushnumber(L, robot->battle.rank);
		lua_setfield(L, -2, "rank");

		return 1;
	}

	// No arguments to return
	return 0;
}

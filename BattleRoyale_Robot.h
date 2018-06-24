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

#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


class OneLoneCoder_BattleRoyale;

enum COMMANDS
{
	CMD_NOTHING = 0,
	CMD_FORWARDS = 1,
	CMD_BACKWARDS = 2,
	CMD_TURNLEFT = 3,
	CMD_TURNRIGHT = 4,
	CMD_FIRE = 5,
	CMD_SHIELD = 6,
	CMD_MINE = 7,
	CMD_DESTROY = 8,
	CMD_TURNANGLE = 9,
	CMD_CLOAK = 10,
};

struct sWall
{
	float sx, sy;
	float ex, ey;
};


struct sBullet
{
	float x;
	float y;
	float vx;
	float vy;
	int owner;
	bool bDead = false;
};


class cRobot
{
public:
	struct STATUS
	{
		float posx = 150.0f;
		float posy = 150.0f;
		float angle = 0.0f;
		float speed = 0.0f;
		int health = 10;
		int shields = 3;
		float shieldactive = 0.0f;
		int cloaks = 3;
		float cloakactive = 0.0f;
		float cooldown = 0.0f;
		int id = 0;
		float fMoveSpeed = 30.0f;
		float fTurnSpeed = 3.14159f / 2.0f;
		short nColour = 0;
		std::string name;
		std::string script;
	} status;

	struct BATTLE
	{
		float gametime = 0.0f;
		int opponents = 0;
		int rank = 0;
	} battle;

	struct WALL_SENSOR
	{
		float n = 2000.0f;
		float s = 2000.0f;
		float w = 2000.0f;
		float e = 2000.0f;
	} wall;

	struct ENEMY_SENSOR
	{
		float n = 2000.0f;
		float s = 2000.0f;
		float w = 2000.0f;
		float e = 2000.0f;
	} enemy;

	struct MAP_SENSOR
	{
		struct MAP_ENTRY
		{
			int id;
			float x;
			float y;
			float distance;
		};

		std::vector<MAP_ENTRY> entries;
	} map;

	enum STATES
	{
		STATE_WAIT_FOR_COMMAND,
		STATE_COMMAND_COMPLETE,
		STATE_MOVING_FORWARDS,
		STATE_MOVING_BACKWARDS,
		STATE_TURNING_LEFT,
		STATE_TURNING_RIGHT,
		STATE_TURNING_ANGLE,
	} nCurrentState, nNextState;

public:
	cRobot();
	cRobot(const cRobot& r);
	
	std::string Test(std::string sFile);
	void Start();
	void LuaScript(std::string sFile, int start_delay);

	COMMANDS nCommandToExecute = CMD_NOTHING;
	float fArgument = 0.0f;
	float fCommandTimeRemaining = 0.0f;
	std::condition_variable cvExecutingCommand;
	std::mutex muxExecutingCommand;
	std::atomic<bool> bPerformingAction;
	std::mutex muxUpdatingSensors;
	std::thread threadStateMachine, threadLuaScript;

	void UpdateStateMachine(float fElapsedTime, OneLoneCoder_BattleRoyale* pHost);

	static int lua_PerformAction(lua_State *L);
	static int lua_ReadSensors(lua_State *L);
};
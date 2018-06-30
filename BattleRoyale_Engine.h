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
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <cmath>

#include "BattleRoyale_Robot.h"

struct sAnnouncement
{
	int id;
	enum COMMENT
	{
		ANNOUNCE_START,
		ANNOUNCE_END,
		ANNOUNCE_ELIMINATION,
		ANNOUNCE_MALFUNCTION,
		ANNOUNCE_TERMINATION,
	} comment;
};

class OneLoneCoder_BattleRoyale
{
public:
	OneLoneCoder_BattleRoyale();


public:
	// Returns a vector full of robots in arena
	std::vector<cRobot*>& GetRobots();

	// Returns a vector filled with "lines" that represent walls
	std::vector<sWall>& GetWalls();

	// Returns a vector filled with bullets and locations
	std::vector<sBullet>& GetBullets();

	// Add a wall (in the form of a line) to the arena
	void AddWall(float x1, float y1, float x2, float y2);

	// Calls the battle manager to update with dt
	void Update(float fElapsedTime);

	// Test the LUA script for errors
	std::string TestRobot(std::string sBotFile);

	// Add the robot to the arena
	std::string AddRobot(std::string sBotFile, int nTeamID);

	// Start the battle
	void Start();

	// Adds a bullet to the arena
	void AddBullet(float x, float y, float vx, float vy, int owner);

	// Get length of battle so far in seconds
	float GetBattleDuration();

	// Get 
	bool IsBattleStarted();
	bool IsBattleOver();
	bool IsAnnouncement();

	sAnnouncement GetAnnouncement();
	

private:
	float fBattleDuration = 0.0f;
	bool bBattleStarted = false;
	bool bBattleOver = false;
	int nWinningRobot = 0;
	float tx, ty;
	std::vector<cRobot*> vecRobots;
	std::vector<sWall> vecWalls;
	std::vector<sBullet> vecBullets;
	std::list<sAnnouncement> listAnnouncements;
	int nArenaWidth = 200;
	int nArenaHeight = 200;
	std::atomic<float> fBattleTime{ 0.0f };
	std::mutex muxBullets;
	float DistanceToLineSegment(float x1, float y1, float x2, float y2, sWall &edge);
};


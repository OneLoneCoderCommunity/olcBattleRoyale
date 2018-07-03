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
#include <cmath>
#include <tuple>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


class BattleRoyale_Parameters
{
public:
	BattleRoyale_Parameters();
	~BattleRoyale_Parameters();

	bool LoadParameters(std::string sFile);

	static bool bDebugMode;
	static int nMaxRobots;
	static int nMaxRobotHealth;
	static float fMaxRobotEnergy;
	static float fRobotMoveSpeed;
	static float fRobotMoveTime;
	static float fRobotTurnSpeed;
	static float fRobotTurnTime;
	static float fMaxGameTime;
	static float fGunCooldown;
	static int nBulletDamage;
	static float fBulletSpeed;
	static int nSelfDestructBullets;
	static float fCollisionRadius;
	static bool bAllowFriendlyFire;


	static int nBattleMode;
	static std::vector<std::pair<int, std::string>> vecRobots;
	static std::vector<std::string> vecTeamNames;
	static std::vector<std::tuple<float, float, float, float>> vecWalls;
};


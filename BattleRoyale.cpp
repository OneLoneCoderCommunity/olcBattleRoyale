/*
OneLoneCoder.com - Battle Royale!
"HUNT. KILL. EAT. " - @Javidx9

License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:
https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9
GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE

From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!

Background
~~~~~~~~~~
Let's kill each other with robots. Level playing field, you script your robot
to survive in battle. last robot alive wins. Simple.

Author
~~~~~~
Twitter: @javidx9
Blog: http://www.onelonecoder.com
Discord: https://discord.gg/WhwHUMV


Last Updated: 14/06/2018
*/

#include "olcConsoleGameEngine.h"
#include <string>
#include <algorithm>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


enum COMMANDS
{
	CMD_NOTHING		= 0,
	CMD_FORWARDS	= 1,
	CMD_BACKWARDS	= 2,
	CMD_TURNLEFT	= 3,
	CMD_TURNRIGHT	= 4,
	CMD_FIRE		= 5,
	CMD_SHIELD		= 6,
	CMD_MINE		= 7,
	CMD_DESTROY		= 8,
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
		int id = 0;
		float fMoveSpeed = 30.0f;
		float fTurnSpeed = 3.14159f / 2.0f;
		short nColour = FG_RED;
		string name;
	} status;

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

		vector<MAP_ENTRY> entries;
	} map;

	enum STATES
	{
		STATE_WAIT_FOR_COMMAND,
		STATE_COMMAND_COMPLETE,
		STATE_MOVING_FORWARDS,
		STATE_MOVING_BACKWARDS,
		STATE_TURNING_LEFT,
		STATE_TURNING_RIGHT,
	} nCurrentState, nNextState;

public:
	cRobot()
	{

	}

	cRobot(const cRobot& r)
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
		
	}

	thread threadStateMachine, threadLuaScript;

	string Test(string sFile)
	{
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);
		lua_register(L, "_PerformAction", lua_PerformAction);
		lua_register(L, "_ReadSensors", lua_ReadSensors);

		int r = luaL_loadfile(L, sFile.c_str());
		if (r != LUA_OK)
		{
			string errormsg = lua_tostring(L, -1);
			lua_close(L);
			return errormsg;
		}

		int i = lua_pcall(L, 0, LUA_MULTRET, 0);
		if (i != LUA_OK)
		{
			string errormsg = lua_tostring(L, -1);
			lua_close(L);
			return errormsg;
		}

		return "OK";
	}
	
	void Start(string sFile)
	{
		nNextState = nCurrentState = STATE_WAIT_FOR_COMMAND;
		threadLuaScript = thread(&cRobot::LuaScript, this, sFile, rand() % 1000);
	}

	void LuaScript(string sFile, int start_delay)
	{
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);

		
		


		lua_register(L, "_PerformAction", lua_PerformAction);
		lua_register(L, "_ReadSensors", lua_ReadSensors);


		int r = luaL_loadfile(L, "javidbot.lua");
		if (r != LUA_OK)
		{
			string errormsg = lua_tostring(L, -1);
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
		this_thread::sleep_for(chrono::milliseconds(start_delay));

		while (1)
		{
			lua_getglobal(L, "Update");
			if (lua_isfunction(L, -1))
			{				
				if (lua_pcall(L, 0, 0, 0) == LUA_OK)
				{
					
				}
				else
				{

				}
			}
			else
			{
	
			}

			lua_settop(L, 0);
			//this_thread::sleep_for(chrono::milliseconds(10));
		}
	}

	

	COMMANDS nCommandToExecute = CMD_NOTHING;
	float fCommandTimeRemaining = 0.0f;
	condition_variable cvExecutingCommand;
	mutex muxExecutingCommand;
	atomic<bool> bPerformingAction;

	mutex muxUpdatingSensors;

	void UpdateStateMachine(float fElapsedTime)
	{
	
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
					fCommandTimeRemaining = 0.5f;
					//bPerformingAction = true;
					nNextState = STATE_MOVING_FORWARDS;
					break;

				case CMD_BACKWARDS:
					nCommandToExecute = CMD_NOTHING;
					fCommandTimeRemaining = 0.5f;
					//bPerformingAction = true;
					nNextState = STATE_MOVING_BACKWARDS;
					break;

				case CMD_TURNLEFT:
					nCommandToExecute = CMD_NOTHING;
					fCommandTimeRemaining = 0.5f;
					//bPerformingAction = true;
					nNextState = STATE_TURNING_LEFT;
					break;

				case CMD_TURNRIGHT:
					nCommandToExecute = CMD_NOTHING;
					fCommandTimeRemaining = 0.5f;
					//bPerformingAction = true;
					nNextState = STATE_TURNING_RIGHT;
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
				unique_lock<mutex> lm(muxExecutingCommand);
				cvExecutingCommand.notify_one();
				break;


			}

			nCurrentState = nNextState;

	}

	static int lua_PerformAction(lua_State *L)
	{
		// Get pointer to this class
		cRobot* robot = (cRobot*)lua_touserdata(L, 1);
		
		// Get Command identifier
		int nCommand = lua_tointeger(L, 2);

		// Get optional argument 1
		float fArg1 = lua_tonumber(L, 3);

		// Call appropriate funtion
		robot->nCommandToExecute = (COMMANDS)nCommand;
		
		// Block thread until command complete signal
		unique_lock<mutex> lm(robot->muxExecutingCommand);
		robot->bPerformingAction = true;
		while(robot->bPerformingAction)
			robot->cvExecutingCommand.wait(lm);

		// No arguments to return
		return 0;
	}

	static int lua_ReadSensors(lua_State *L)
	{
		// Get pointer to this class
		cRobot* robot = (cRobot*)lua_touserdata(L, 1);

		// Get Command identifier
		int nSensorBank = lua_tointeger(L, 2);

		if (nSensorBank == 0) // Wall
		{
			unique_lock<mutex> lm(robot->muxUpdatingSensors);
			lua_pushnumber(L, robot->wall.n);
			lua_pushnumber(L, robot->wall.e);
			lua_pushnumber(L, robot->wall.s);
			lua_pushnumber(L, robot->wall.w);
			return 4;
		}

		if (nSensorBank == 1) // Enemy
		{
			unique_lock<mutex> lm(robot->muxUpdatingSensors);
			lua_pushnumber(L, robot->enemy.n);
			lua_pushnumber(L, robot->enemy.e);
			lua_pushnumber(L, robot->enemy.s);
			lua_pushnumber(L, robot->enemy.w);
			return 4;
		}

		// No arguments to return
		
	}
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


class OneLoneCoder_BattleRoyale: public olcConsoleGameEngine
{
public:
	OneLoneCoder_BattleRoyale()
	{
		m_sAppName = L"Battle Royale!";
	}

private:

	vector<cRobot> vecRobots;
	vector<sWall> vecWalls;
	vector<sBullet> vecBullets;

	vector<pair<float, float>> vecRobotModel;

	int nArenaWidth = 200;
	int nArenaHeight = 200;

	olcSprite *sprFont = nullptr;


	void DrawBigText(string sText, int x, int y)
	{
		int i = 0;
		for (auto c : sText)
		{
			int sx = ((c - 32) % 16) * 8;
			int sy = ((c - 32) / 16) * 8;
			DrawPartialSprite(x + i * 8, y, sprFont, sx, sy, 8, 8);
			i++;
		}
	}

protected:

	virtual bool OnUserCreate()
	{
		vecRobotModel = {
			{-1.0f, -1.0f}, {-1.0f, 0.0f}, {0.0f, 2.0f}, {1.0f, 0.0f}, {1.0f, -1.0f}
		};

		vecWalls = {
			{ 0.0f, 0.0f, 200.0f, 0.0f },
			{ 0.0f, 200.0f, 200.0f, 200.0f },
			{ 0.0f, 0.0f, 0.0f, 200.0f },
			{ 200.0f, 0.0f, 200.0f, 200.0f },


			//{ 50.0f, 50.0f, 150.0f, 50.0f },
			//{ 50.0f, 150.0f, 150.0f, 150.0f },
			//{ 50.0f, 50.0f, 50.0f, 150.0f },
			//{ 150.0f, 50.0f, 150.0f, 150.0f },
		};

		for (int i = 0; i <8; i++)
		{
			cRobot robot;
			robot.status.id = i;
			robot.status.posx = rand() % 200;
			robot.status.posy = rand() % 200;
			robot.status.angle = ((float)rand() / (float)RAND_MAX) * 3.14159f * 2.0f;
			robot.status.nColour = i+1;
			vecRobots.push_back(robot);
		}


		sprFont = new olcSprite(L"javidx9_nesfont8x8.spr");
		
		return true;
	}

	float tx, ty;

	float DistanceToLineSegment(float x1, float y1, float x2, float y2, sWall &edge)
	{
		float x3 = edge.sx;
		float y3 = edge.sy;
		float x4 = edge.ex;
		float y4 = edge.ey;

		float d = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
		float ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / d;
		float ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / d;

		float x =  ua * (x2 - x1);
		float y =  ua * (y2 - y1);
		if (ua >= 0 && ua < 1 && ub >= 0 && ub < 1)
		{
			tx = x + x1;
			ty = y + y1;
			return sqrtf((tx - x1)*(tx - x1) + (ty - y1)*(ty - y1));
		}

		return 2000.0f;	
	}

	bool bDisplayError = false;
	string sErrorMessage;

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

		if (bDisplayError)
		{
			DrawBigText(sErrorMessage, 0, 0);
			return true;
		}

		if (GetKey(VK_SPACE).bReleased)
		{
			// Check all scripts compile
			for (auto &robot : vecRobots)
			{
				string s = robot.Test("javidbot.lua");
				
				if(s != "OK")
				{
					sErrorMessage = s;
					bDisplayError = true;
				}
			}

			// Start robot threads
			for (auto &robot : vecRobots)
			{
				robot.Start("javidbot.lua");
			}

		}

		// Update Bullets
		for(auto &bullet : vecBullets)
		{
			bullet.x += bullet.vx * 40.0f * fElapsedTime;
			bullet.y += bullet.vy * 40.0f * fElapsedTime;

			if (bullet.x<0.0f || bullet.x > 200.0f || bullet.y <0.0f || bullet.y > 200.0f)
				bullet.bDead = true;
		}

		vecBullets.erase(remove_if(vecBullets.begin(), vecBullets.end(), [](sBullet &o) {return o.bDead; }), vecBullets.end());

		// Update Robots
		for (auto &robot : vecRobots)
		{
			

			
			// Assess environment
			robot.muxUpdatingSensors.lock();
			// Check for wall proximity
			robot.wall.n = 2000.0f;
			robot.wall.s = 2000.0f;
			robot.wall.e = 2000.0f;
			robot.wall.w = 2000.0f;

			float fProbeNX = 1000.0f * cosf(robot.status.angle) + robot.status.posx;
			float fProbeNY = 1000.0f * sinf(robot.status.angle) + robot.status.posy;

			float fProbeSX = 1000.0f * cosf(robot.status.angle + 3.14159f) + robot.status.posx;
			float fProbeSY = 1000.0f * sinf(robot.status.angle + 3.14159f) + robot.status.posy;

			float fProbeWX = 1000.0f * cosf(robot.status.angle - (3.14159f / 2.0f)) + robot.status.posx;
			float fProbeWY = 1000.0f * sinf(robot.status.angle - (3.14159f / 2.0f)) + robot.status.posy;

			float fProbeEX = 1000.0f * cosf(robot.status.angle + (3.14159f / 2.0f)) + robot.status.posx;
			float fProbeEY = 1000.0f * sinf(robot.status.angle + (3.14159f / 2.0f)) + robot.status.posy;

			for (auto wall : vecWalls)
			{

				// North Probe
				float d = DistanceToLineSegment(robot.status.posx, robot.status.posy, fProbeNX, fProbeNY, wall);
				robot.wall.n = min(robot.wall.n, d);

				// North South
				d = DistanceToLineSegment(robot.status.posx, robot.status.posy, fProbeSX, fProbeSY, wall);
				robot.wall.s = min(robot.wall.s, d);

				// West
				d = DistanceToLineSegment(robot.status.posx, robot.status.posy, fProbeWX, fProbeWY, wall);
				robot.wall.w = min(robot.wall.w, d);

				// East
				d = DistanceToLineSegment(robot.status.posx, robot.status.posy, fProbeEX, fProbeEY, wall);
				robot.wall.e = min(robot.wall.e, d);
			}

			// Check for enemy proximity
			robot.enemy.n = 2000.0f;
			robot.enemy.s = 2000.0f;
			robot.enemy.w = 2000.0f;
			robot.enemy.e = 2000.0f;

			// For each enemy, assess which quadrant it is in, and work out closest
			// distance for that quadrant

			// Clear map
			robot.map.entries.clear();
			
			// Coarse quadrant detection
			for (auto &enemy : vecRobots)
			{
				if (enemy.status.id != robot.status.id) // Dont check against self
				{
					float fAngle = atan2f(enemy.status.posy - robot.status.posy, enemy.status.posx - robot.status.posx);
					float d = sqrtf(powf((enemy.status.posx - robot.status.posx), 2.0f) + powf((enemy.status.posy - robot.status.posy), 2.0f));
					if (fAngle >= -3.14159f * 0.25f && fAngle < 3.14159f * 0.25f)
						robot.enemy.n = min(robot.enemy.n, d);
					if (fAngle >= 3.14159f * 0.25f && fAngle < 3.14159f * 0.75f)
						robot.enemy.e = min(robot.enemy.e, d);
					if (fAngle < -3.14159f * 0.25f && fAngle >= -3.14159f * 0.75f)
						robot.enemy.s = min(robot.enemy.s, d);
					if (fAngle >= 3.14159f * 0.75f || fAngle < -3.14159f * 0.75f)
						robot.enemy.w = min(robot.enemy.w, d);

					robot.map.entries.push_back({ enemy.status.id, enemy.status.posx, enemy.status.posy, d });
				}
			}

			// Sort map entries by distance
			sort(robot.map.entries.begin(), robot.map.entries.end(), [](const cRobot::MAP_SENSOR::MAP_ENTRY &r1, const cRobot::MAP_SENSOR::MAP_ENTRY &r2)
			{return r1.distance < r2.distance; });
			
			robot.muxUpdatingSensors.unlock();
			
			robot.UpdateStateMachine(fElapsedTime);

			// Collision Handle
			if (robot.status.posx < 0.0f) robot.status.posx = 0.5f;
			if (robot.status.posx >= nArenaWidth) robot.status.posx = (float)nArenaWidth - 0.5f;
			if (robot.status.posy < 0.0f) robot.status.posy = 0.5f;
			if (robot.status.posy >= nArenaHeight) robot.status.posy = (float)nArenaHeight - 0.5f;

			DrawLine(robot.status.posx, robot.status.posy, robot.wall.n * cosf(robot.status.angle) + robot.status.posx,						robot.wall.n * sinf(robot.status.angle) + robot.status.posy, PIXEL_SOLID, FG_RED);
			DrawLine(robot.status.posx, robot.status.posy, robot.wall.s * cosf(robot.status.angle + 3.14159f) + robot.status.posx,				robot.wall.s * sinf(robot.status.angle + 3.14159f) + robot.status.posy, PIXEL_SOLID, FG_RED);
			DrawLine(robot.status.posx, robot.status.posy, robot.wall.w * cosf(robot.status.angle - (3.14159f / 2.0f)) + robot.status.posx,	robot.wall.w * sinf(robot.status.angle - (3.14159f / 2.0f)) + robot.status.posy, PIXEL_SOLID, FG_RED);
			DrawLine(robot.status.posx, robot.status.posy, robot.wall.e * cosf(robot.status.angle + (3.14159f / 2.0f)) + robot.status.posx,	robot.wall.e * sinf(robot.status.angle + (3.14159f / 2.0f)) + robot.status.posy, PIXEL_SOLID, FG_RED);

		}

		// Draw Arena
		for(auto &wall: vecWalls)
			DrawLine(wall.sx, wall.sy, wall.ex, wall.ey, PIXEL_SOLID, FG_WHITE);
		

		// Draw Robots
		for (auto &robot : vecRobots)
		{
			DrawWireFrameModel(vecRobotModel, robot.status.posx, robot.status.posy, robot.status.angle - (3.14159f / 2.0f), 3.0f, robot.status.nColour);
		}

		// Draw Bullets
		for (auto &bullet : vecBullets)
		{
			Draw(bullet.x, bullet.y, PIXEL_SOLID, FG_WHITE);
		}

		// Draw Mines

		// Draw HUD

		int nList = 0;
		string sDisplay = "";
		for (auto &robot : vecRobots)
		{
			sDisplay = to_string(robot.status.id) + ": " + robot.status.name;
			DrawBigText(sDisplay, 210, nList * 10 + 2);
			nList++;

			switch (robot.nCurrentState)
			{
			case cRobot::STATES::STATE_WAIT_FOR_COMMAND:
				sDisplay = "IDLE";
				break;
			case cRobot::STATES::STATE_MOVING_FORWARDS:
				sDisplay = "FORWARDS";
				break;
			case cRobot::STATES::STATE_MOVING_BACKWARDS:
				sDisplay = "BACKWARDS";
				break;
			case cRobot::STATES::STATE_TURNING_LEFT:
				sDisplay = "TURNING LEFT";
				break;
			case cRobot::STATES::STATE_TURNING_RIGHT:
				sDisplay = "TURNING_RIGHT";
				break;
			}

			DrawBigText(sDisplay, 210, nList * 10 + 2);
			nList++;
			nList++;
		}

		return true;
	}
};


int main()
{
	OneLoneCoder_BattleRoyale demo;
	demo.ConstructConsole(320, 240, 4, 4);
	demo.Start();

	return 0;
}
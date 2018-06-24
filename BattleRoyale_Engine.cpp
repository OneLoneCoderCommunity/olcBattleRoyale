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

#include "BattleRoyale_Engine.h"


OneLoneCoder_BattleRoyale::OneLoneCoder_BattleRoyale()
{

}

void OneLoneCoder_BattleRoyale::AddBullet(float x, float y, float vx, float vy, int owner)
{
	std::unique_lock<std::mutex> lm(muxBullets);
	vecBullets.push_back({ x, y, vx, vy, owner, false });
}

void OneLoneCoder_BattleRoyale::AddWall(float x1, float y1, float x2, float y2)
{
	vecWalls.push_back({ x1, y1, x2, y2 });
}

float OneLoneCoder_BattleRoyale::DistanceToLineSegment(float x1, float y1, float x2, float y2, sWall &edge)
{
	float x3 = edge.sx;
	float y3 = edge.sy;
	float x4 = edge.ex;
	float y4 = edge.ey;

	float d = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
	float ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / d;
	float ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / d;

	float x = ua * (x2 - x1);
	float y = ua * (y2 - y1);
	if (ua >= 0 && ua < 1 && ub >= 0 && ub < 1)
	{
		tx = x + x1;
		ty = y + y1;
		return sqrtf((tx - x1)*(tx - x1) + (ty - y1)*(ty - y1));
	}

	return 2000.0f;
}

void OneLoneCoder_BattleRoyale::Update(float fElapsedTime)
{	
	// Update Bullets
	for (auto &bullet : vecBullets)
	{
		bullet.x += bullet.vx * 100.0f * fElapsedTime;
		bullet.y += bullet.vy * 100.0f * fElapsedTime;

		// Has bullet left arena?
		if (bullet.x<0.0f || bullet.x > 200.0f || bullet.y <0.0f || bullet.y > 200.0f)
			bullet.bDead = true;

		// Has bullet hit a robot?
		for (auto &robot : vecRobots)
		{
			// Ignore if bullet was fired by owner of bullet (can't shoot yourself!)
			if (bullet.owner != robot->status.id)
			{
				// Ignore if robot has shields raised
				if (robot->status.shieldactive <= 0.0f)
				{
					if (powf(bullet.x - robot->status.posx, 2.0f) + powf(bullet.y - robot->status.posy, 2.0f) <= 16.0f)
					{
						// Bullet has hit robot
						robot->muxUpdatingSensors.lock();
							robot->status.health--;
							if (robot->status.health < 0) robot->status.health = 0;
						robot->muxUpdatingSensors.unlock();

						// Kill Bullet
						bullet.bDead = true;
					}
				}
			}
		}
	}

	// Remove bullets that have expired
	vecBullets.erase(remove_if(vecBullets.begin(), vecBullets.end(), [](sBullet &o) {return o.bDead; }), vecBullets.end());

	// Sort robot based on health
	sort(vecRobots.begin(), vecRobots.end(), [](const cRobot* r1, const cRobot* r2) {return r1->status.health > r2->status.health; });

	// Count living robot
	int nAliveRobots = count_if(vecRobots.begin(), vecRobots.end(), [](const cRobot* r1) {return r1->status.health > 0; });

	// Update Robots
	for (auto &robot : vecRobots)
	{
		// Only if robots are functional
		if (robot->status.health > 0)
		{
			// Lock the robot whilst most of its properties are written to
			robot->muxUpdatingSensors.lock();

			// Assess environment

			// Check for wall proximity
			robot->wall.n = 2000.0f;
			robot->wall.s = 2000.0f;
			robot->wall.e = 2000.0f;
			robot->wall.w = 2000.0f;

			float fProbeNX = 1000.0f * cosf(robot->status.angle) + robot->status.posx;
			float fProbeNY = 1000.0f * sinf(robot->status.angle) + robot->status.posy;

			float fProbeSX = 1000.0f * cosf(robot->status.angle + 3.14159f) + robot->status.posx;
			float fProbeSY = 1000.0f * sinf(robot->status.angle + 3.14159f) + robot->status.posy;

			float fProbeWX = 1000.0f * cosf(robot->status.angle - (3.14159f / 2.0f)) + robot->status.posx;
			float fProbeWY = 1000.0f * sinf(robot->status.angle - (3.14159f / 2.0f)) + robot->status.posy;

			float fProbeEX = 1000.0f * cosf(robot->status.angle + (3.14159f / 2.0f)) + robot->status.posx;
			float fProbeEY = 1000.0f * sinf(robot->status.angle + (3.14159f / 2.0f)) + robot->status.posy;

			for (auto wall : vecWalls)
			{
				// North Probe
				float d = DistanceToLineSegment(robot->status.posx, robot->status.posy, fProbeNX, fProbeNY, wall);
				robot->wall.n = std::min(robot->wall.n, d);

				// North South
				d = DistanceToLineSegment(robot->status.posx, robot->status.posy, fProbeSX, fProbeSY, wall);
				robot->wall.s = std::min(robot->wall.s, d);

				// West
				d = DistanceToLineSegment(robot->status.posx, robot->status.posy, fProbeWX, fProbeWY, wall);
				robot->wall.w = std::min(robot->wall.w, d);

				// East
				d = DistanceToLineSegment(robot->status.posx, robot->status.posy, fProbeEX, fProbeEY, wall);
				robot->wall.e = std::min(robot->wall.e, d);
			}

			// Check for enemy proximity
			robot->enemy.n = 2000.0f;
			robot->enemy.s = 2000.0f;
			robot->enemy.w = 2000.0f;
			robot->enemy.e = 2000.0f;

			// For each enemy, assess which quadrant it is in, and work out closest
			// distance for that quadrant

			// Clear map
			robot->map.entries.clear();

			// Coarse quadrant detection
			for (auto &enemy : vecRobots)
			{
				if (enemy->status.id != robot->status.id) // Dont check against self
				{
					if (enemy->status.cloakactive <= 0.0f && enemy->status.health > 0)
					{
						float fAngle = atan2f(enemy->status.posy - robot->status.posy, enemy->status.posx - robot->status.posx);
						float d = sqrtf(powf((enemy->status.posx - robot->status.posx), 2.0f) + powf((enemy->status.posy - robot->status.posy), 2.0f));
						if (fAngle >= -3.14159f * 0.25f && fAngle < 3.14159f * 0.25f)
							robot->enemy.n = std::min(robot->enemy.n, d);
						if (fAngle >= 3.14159f * 0.25f && fAngle < 3.14159f * 0.75f)
							robot->enemy.e = std::min(robot->enemy.e, d);
						if (fAngle < -3.14159f * 0.25f && fAngle >= -3.14159f * 0.75f)
							robot->enemy.s = std::min(robot->enemy.s, d);
						if (fAngle >= 3.14159f * 0.75f || fAngle < -3.14159f * 0.75f)
							robot->enemy.w = std::min(robot->enemy.w, d);

						robot->map.entries.push_back({ enemy->status.id, enemy->status.posx, enemy->status.posy, d });
					}
				}
			}

			// Add yourself to map at element 0
			robot->map.entries.push_back({ robot->status.id, robot->status.posx, robot->status.posy, 0 });

			// Sort map entries by distance
			sort(robot->map.entries.begin(), robot->map.entries.end(), [](const cRobot::MAP_SENSOR::MAP_ENTRY &r1, const cRobot::MAP_SENSOR::MAP_ENTRY &r2)
			{return r1.distance < r2.distance; });

			// Update game state for robot
			robot->battle.gametime += fElapsedTime;
			robot->battle.opponents = nAliveRobots;

			// Robots are ranked according to health
			robot->battle.rank = distance(vecRobots.begin(), find_if(vecRobots.begin(), vecRobots.end(), [robot](const cRobot *r1) { return r1->status.id == robot->status.id; }));

			// Done with writing to robot
			robot->muxUpdatingSensors.unlock();

			// Update the robots mechanics
			robot->UpdateStateMachine(fElapsedTime, this);

			// Collision Handle
			if (robot->status.posx < 0.0f) robot->status.posx = 0.5f;
			if (robot->status.posx >= nArenaWidth) robot->status.posx = (float)nArenaWidth - 0.5f;
			if (robot->status.posy < 0.0f) robot->status.posy = 0.5f;
			if (robot->status.posy >= nArenaHeight) robot->status.posy = (float)nArenaHeight - 0.5f;

			//DrawLine(robot->status.posx, robot->status.posy, robot->wall.n * cosf(robot->status.angle) + robot->status.posx, robot->wall.n * sinf(robot->status.angle) + robot->status.posy, PIXEL_SOLID, FG_RED);
			//DrawLine(robot->status.posx, robot->status.posy, robot->wall.s * cosf(robot->status.angle + 3.14159f) + robot->status.posx, robot->wall.s * sinf(robot->status.angle + 3.14159f) + robot->status.posy, PIXEL_SOLID, FG_RED);
			//DrawLine(robot->status.posx, robot->status.posy, robot->wall.w * cosf(robot->status.angle - (3.14159f / 2.0f)) + robot->status.posx, robot->wall.w * sinf(robot->status.angle - (3.14159f / 2.0f)) + robot->status.posy, PIXEL_SOLID, FG_RED);
			//DrawLine(robot->status.posx, robot->status.posy, robot->wall.e * cosf(robot->status.angle + (3.14159f / 2.0f)) + robot->status.posx, robot->wall.e * sinf(robot->status.angle + (3.14159f / 2.0f)) + robot->status.posy, PIXEL_SOLID, FG_RED);
		}
	}
}

std::vector<cRobot*>& OneLoneCoder_BattleRoyale::GetRobots()
{
	return vecRobots;
}

std::vector<sWall>& OneLoneCoder_BattleRoyale::GetWalls()
{
	return vecWalls;
}

std::vector<sBullet>& OneLoneCoder_BattleRoyale::GetBullets()
{
	return vecBullets;
}


std::string OneLoneCoder_BattleRoyale::TestRobot(std::string sBotFile)
{
	cRobot r;
	return r.Test(sBotFile);	
}

std::string OneLoneCoder_BattleRoyale::AddRobot(std::string sBotFile)
{
	// Create new robot 
	cRobot* robot = new cRobot();
	robot->status.id = vecRobots.size();
	robot->status.posx = (float)(rand() % 200);
	robot->status.posy = (float)(rand() % 200);
	robot->status.angle = ((float)rand() / (float)RAND_MAX) * 3.14159f * 2.0f;
	robot->status.nColour = robot->status.id + 8;
	robot->status.script = sBotFile;

	// Add robot to arena
	vecRobots.push_back(robot);

	return "OK";
}

void OneLoneCoder_BattleRoyale::Start()
{
	for (auto& robot : vecRobots)
		robot->Start();
}
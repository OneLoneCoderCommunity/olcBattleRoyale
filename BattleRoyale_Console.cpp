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

#include "BattleRoyale_Console.h"

OneLoneCoder_BattleRoyaleConsole::OneLoneCoder_BattleRoyaleConsole()
{
	// Give the OLCCGE a title
	m_sAppName = L"Battle Royale!";
}

bool OneLoneCoder_BattleRoyaleConsole::OnUserCreate()
{
	// STEP 1) Configure Battle Engine Parameters
	// This needs to be performed before any robots are loaded
	// into the OLCBRE
	engine.AddWall(0.0f, 0.0f, 200.0f, 0.0f);
	engine.AddWall(0.0f, 200.0f, 200.0f, 200.0f);
	engine.AddWall(0.0f, 0.0f, 0.0f, 200.0f);
	engine.AddWall(200.0f, 0.0f, 200.0f, 200.0f);

	// These are just console only graphics resources
	sprFont = new olcSprite(L"javidx9_nesfont8x8.spr");
	vecRobotModel = {
		{ -1.0f, -1.0f },{ -1.0f, 0.0f },{ 0.0f, 2.0f },{ 1.0f, 0.0f },{ 1.0f, -1.0f }
	};

	return true;
}

bool OneLoneCoder_BattleRoyaleConsole::OnUserUpdate(float fElapsedTime)
{
	// Claer the console
	Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

	// If a lua script has had an error, this will be all we draw
	// so it can be read
	if (bDisplayError)
	{
		DrawBigText(sErrorMessage, 0, 0);
		return true;
	}

	// else


	// STEP 2) Depending on your interface this will vary, but this code 
	// demonstrates testing and loading botfiles (lua scripts) into OLCBRE
	if (IsFocused())
	{
		if (GetKey(VK_SPACE).bReleased)
		{
			if (!engine.IsBattleStarted())
			{				
				ifstream data("battle.txt", ios::in | ios::binary);
				vector<string> botFiles;
				if (data.is_open())
				{
					while (!data.eof() && botFiles.size() < 8)
					{
						string s;
						data >> s;
						botFiles.push_back(s);
					}
				}
				
				 //= { "JavidBot.lua", "NullBot.lua" };// , "BrankBot.lua", "NullBot.lua", "JavidBot.lua", "NullBot.lua", "JavidBot.lua", "NullBot.lua"
			

				for (auto &s : botFiles)
				{
					// If the script has syntax errors, they will be returned here
					sErrorMessage = engine.TestRobot(s);

					// If there are none, it will return OK
					if (sErrorMessage != "OK")
					{
						bDisplayError = true;
						break;
					}
					else
					{
						// So STEP 3) Add the error free robot to the OLCBRE
						engine.AddRobot(s);
					}
				}
			}

			// STEP 4) Once all the robots are loaded, start the battle!
			if(!bDisplayError)
				engine.Start();
		}

		//if (GetKey(L'Z').bReleased)
		//{
			
		//}
	}

	// STEP 5) This needs to be called during your game loop, ideally per frame
	// with the elapsed time since the last time it was called
	engine.Update(fElapsedTime);


	// STEP 6) Draw The Graphics! Everything below here is geared towards
	// displaying OLCBRE in the command line using the olcConsoleGameEngine.
	// Your interface may require different things, like 3D models and lighting
	// or whatever, it doesnt matter. What you can see here though are the things
	// that need drawing

	// Draw Arena - Defined by walls
	for (auto &wall : engine.GetWalls())
		DrawLine((int)wall.sx, (int)wall.sy, (int)wall.ex, (int)wall.ey, PIXEL_SOLID, FG_WHITE);

	// Draw Robots - All attributes of robots are available for determining how they
	// should be drawn. Just DONT WRITE to them!
	for (auto const &robot : engine.GetRobots())
	{
		// Robots need a visible shield when it is deployed
		if (robot->status.shieldactive > 0.0f)
			DrawCircle((int)robot->status.posx, (int)robot->status.posy, (int)7.0f, PIXEL_HALF, FG_CYAN);
		
		// Robots need to indicate they are cloaked, in this case I make them translucent
		if (robot->status.cloakactive > 0.0f)
			DrawWireFrameModel(vecRobotModel, robot->status.posx, robot->status.posy, robot->status.angle - (3.14159f / 2.0f), 3.0f, robot->status.nColour, PIXEL_HALF);
		else
			// Robots need to indicate when they are dead (no health left), which I 
			// do by using a darker shade to display them
			if (robot->status.health == 0)
				DrawWireFrameModel(vecRobotModel, robot->status.posx, robot->status.posy, robot->status.angle - (3.14159f / 2.0f), 3.0f, robot->status.nColour, PIXEL_HALF);			
			else
				// Else robots should be drawn as normal, facing in the direction 
				// of their angle
				DrawWireFrameModel(vecRobotModel, robot->status.posx, robot->status.posy, robot->status.angle - (3.14159f / 2.0f), 3.0f, robot->status.nColour, PIXEL_SOLID);
	}

	// Draw Bullets - there is only one type of projectile. Bullets have
	// a location and a direction vector
	for (auto &bullet : engine.GetBullets())
		Draw((int)bullet.x, (int)bullet.y, PIXEL_SOLID, FG_WHITE);

	// TODO: Draw Mines

	// Draw HUD - It is useful to indicate the current ranking order and some
	// status information about the robots. Here I also use the colour of the
	// robot to highlight the rankings

	// The vector of robots is sorted in order of robot health, from healthiest
	// to dead. Those that have died will remain in the order they have died at 
	// the end of the vector

	int nList = 0;
	string sDisplay = "";
	for (auto &robot : engine.GetRobots())
	{
		Fill(209, nList * 10 + 1, ScreenWidth(), nList * 10 + 22, PIXEL_SOLID, robot->status.nColour - 8);

		sDisplay = robot->status.name + " (" + to_string(robot->status.health) + ")";
		DrawBigText(sDisplay, 210, nList * 10 + 2);
		nList++;

		switch (robot->nCurrentState)
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
			sDisplay = "TURNING RIGHT";
			break;
		}

		DrawBigText(sDisplay, 210, nList * 10 + 2);
		nList++;
		nList++;
	}

	int total = (int)engine.GetBattleDuration();
	int nHours = total / 3600; total %= 3600;
	int nMinutes = total / 60; total %= 60;
	int nSeconds = total;

	sDisplay = "Battle Time: " + to_string(nHours) + "h " + to_string(nMinutes) + "m " + to_string(nSeconds) + "s";
	DrawBigText(sDisplay, 2, 210);

	if (engine.IsBattleOver())
	{
		sDisplay = "WINNER = " + engine.GetRobots()[0]->status.name;
		DrawBigText(sDisplay, 10, 220);
	}
	return true;
}

// Utility function for drawing large text to console
void OneLoneCoder_BattleRoyaleConsole::DrawBigText(string sText, int x, int y)
{
	int i = 0;
	int cx = x;
	int cy = y;
	for (auto c : sText)
	{
		// Find character in map
		int sx = ((c - 32) % 16) * 8;
		int sy = ((c - 32) / 16) * 8;

		DrawPartialSprite(cx, cy, sprFont, sx, sy, 8, 8);
		i++;

		cx += 8;
		if (cx >= ScreenWidth() - 8)
		{
			cy += 8;
			cx = 0;
		}
	}
}
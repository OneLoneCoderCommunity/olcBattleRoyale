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

// This class is just a wrapper that interfaces with the OLC Battle Royale Engine!
// You can create your own interfaces in whatever API you like!

#include "olcConsoleGameEngineOOP.h"
#include "BattleRoyale_Engine.h"

#include <fstream>

class OneLoneCoder_BattleRoyaleConsole : public olcConsoleGameEngineOOP
{
public:
	OneLoneCoder_BattleRoyaleConsole();

private:
	// Instance of the OLCBRE
	OneLoneCoder_BattleRoyale engine;


	// Stuff below here is just to support drawing of the OLCBRE
	// in a windows command prompt
private:
	bool bDisplayError = false;
	string sErrorMessage;
	olcSprite *sprFont = nullptr;
	vector<pair<float, float>> vecRobotModel;
	void DrawBigText(string sText, int x, int y);

protected:
	virtual bool OnUserCreate() override;
	virtual bool OnUserUpdate(float fElapsedTime) override;

};
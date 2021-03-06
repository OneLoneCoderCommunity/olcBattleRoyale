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

#include <fstream>
#include "../Core/BattleRoyale_Engine.h"

#ifdef CGE_OLC
#include "olcConsoleGameEngineOOP.h"
#endif

#ifdef CGE_GL
#include "olcConsoleGameEngineGLOOP.h"
#endif



#ifdef CGE_OLC
class OneLoneCoder_BattleRoyaleConsole : public olcConsoleGameEngineOOP
#endif

#ifdef CGE_GL
class OneLoneCoder_BattleRoyaleConsole : public olcConsoleGameEngineGLOOP
#endif

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

private: // This stuff stores sound samples for announcments
	int sfxStart;
	int sfxPlayerEliminated[8];
	int sfxPlayerDestruct[8];
	int sfxPlayerMalfunction[8];
	int sfxOver;

protected:
	virtual bool OnUserCreate() override;
	virtual bool OnUserUpdate(float fElapsedTime) override;

};
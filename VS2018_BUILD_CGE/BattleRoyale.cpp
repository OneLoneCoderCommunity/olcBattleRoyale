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

#pragma once

#include "BattleRoyale_Console.h"
#include "../Core/BattleRoyale_Parameters.h"

int main(int argc, char** argv)
{
	// Load parameters, either default
	BattleRoyale_Parameters brp;
	
	// Or use file dragged onto exe, or command line args
	if (argc > 1)
	{
		if (!brp.LoadParameters(argv[2]))
		{
			std::cout << "Parameter file invalid" << std::endl;
			return -1;
		}
	}
	else
	{
		if(!brp.LoadParameters("BattleRoyaleRules.lua")) return -1;
	}

	// Launch Simulation
	OneLoneCoder_BattleRoyaleConsole demo;

#ifdef SIZE_BIG
	demo.ConstructConsole(320, 240, 4, 4);
#endif

#ifdef SIZE_SMALL
	demo.ConstructConsole(320, 240, 2, 2);
#endif

	demo.Start();
	return 0;
}
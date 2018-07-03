#include "BattleRoyale_Parameters.h"


bool BattleRoyale_Parameters::bDebugMode = false;
int BattleRoyale_Parameters::nMaxRobots = 8;
int BattleRoyale_Parameters::nMaxRobotHealth = 10;
float BattleRoyale_Parameters::fMaxRobotEnergy = 30.0f;
float BattleRoyale_Parameters::fRobotMoveSpeed = 30.0f;
float BattleRoyale_Parameters::fRobotMoveTime = 0.4f;
float BattleRoyale_Parameters::fRobotTurnSpeed = 3.14159f / 2.0f;
float BattleRoyale_Parameters::fRobotTurnTime = 0.5f;
float BattleRoyale_Parameters::fMaxGameTime = 600.0f;
float BattleRoyale_Parameters::fGunCooldown = 0.2f;
int BattleRoyale_Parameters::nBulletDamage = 1;
float BattleRoyale_Parameters::fBulletSpeed = 100.0f;
int BattleRoyale_Parameters::nSelfDestructBullets = 10;
int BattleRoyale_Parameters::nBattleMode = 1;
float BattleRoyale_Parameters::fCollisionRadius = 4.0f;
bool BattleRoyale_Parameters::bAllowFriendlyFire = false;

std::vector<std::pair<int, std::string>> BattleRoyale_Parameters::vecRobots;
std::vector<std::string> BattleRoyale_Parameters::vecTeamNames;
std::vector<std::tuple<float, float, float, float>> BattleRoyale_Parameters::vecWalls;


BattleRoyale_Parameters::BattleRoyale_Parameters()
{
}


BattleRoyale_Parameters::~BattleRoyale_Parameters()
{
}

bool BattleRoyale_Parameters::LoadParameters(std::string sFile)
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	// Load battle parameter file
	int r = luaL_loadfile(L, sFile.c_str());
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}

	// Execute it
	int i = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (i != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}

	lua_getglobal(L, "DebugMode");
	if (lua_isboolean(L, -1)) BattleRoyale_Parameters::bDebugMode = (bool)lua_toboolean(L, -1);

	lua_getglobal(L, "MaxRobots");
	if (lua_isinteger(L, -1)) BattleRoyale_Parameters::nMaxRobots = (int)lua_tointeger(L, -1);

	lua_getglobal(L, "MaxRobotHealth");
	if (lua_isinteger(L, -1)) BattleRoyale_Parameters::nMaxRobotHealth = (int)lua_tointeger(L, -1);

	lua_getglobal(L, "MaxRobotEnergy");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fMaxRobotEnergy = (float)lua_tonumber(L, -1);

	lua_getglobal(L, "RobotMoveSpeed");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fRobotMoveSpeed= (float)lua_tonumber(L, -1);

	lua_getglobal(L, "RobotMoveTime");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fRobotMoveTime = (float)lua_tonumber(L, -1);

	lua_getglobal(L, "RobotTurnSpeed");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fRobotTurnSpeed = (float)lua_tonumber(L, -1);

	lua_getglobal(L, "RobotTurnTime");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fRobotTurnTime = (float)lua_tonumber(L, -1);

	lua_getglobal(L, "MaxGameTime");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fMaxGameTime = (float)lua_tonumber(L, -1);

	lua_getglobal(L, "GunCooldown");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fGunCooldown = (float)lua_tonumber(L, -1);

	lua_getglobal(L, "BulletSpeed");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fBulletSpeed = (float)lua_tonumber(L, -1);

	lua_getglobal(L, "BulletDamage");
	if (lua_isinteger(L, -1)) BattleRoyale_Parameters::nBulletDamage = (int)lua_tointeger(L, -1);

	lua_getglobal(L, "SelfDestructBullets");
	if (lua_isinteger(L, -1)) BattleRoyale_Parameters::nSelfDestructBullets = (int)lua_tointeger(L, -1);

	lua_getglobal(L, "RobotHitRadius");
	if (lua_isnumber(L, -1)) BattleRoyale_Parameters::fCollisionRadius = (float)lua_tonumber(L, -1);

	lua_getglobal(L, "AllowFriendlyFire");
	if (lua_isboolean(L, -1)) BattleRoyale_Parameters::bAllowFriendlyFire = (bool)lua_toboolean(L, -1);


	// Read in teams
	lua_getglobal(L, "Teams"); // -1 Table "Teams"
	if (lua_istable(L, -1))
	{
		lua_pushnil(L); // -2 Key Nil : -1 Table "Teams"

		while (lua_next(L, -2) != 0) // -1 Table : -2 Key "TeamName" : -3 Table "Teams"
		{
			std::string sTeamName = lua_tostring(L, -2);
			vecTeamNames.push_back(sTeamName);
			if (lua_istable(L, -1)) 
			{
				lua_gettable(L, -1); // -1 Table : -2 Table Value : -3 Key "TeamName" : -4 Table "Teams" 
				lua_pushnil(L);      // -1 Key Nil : -2 Table : -3 Table Value : -4 Key "TeamName" : -5 Table "Teams" 
				while (lua_next(L, -2) != 0) // -1 Value "BotFile" : -2 Key Nil : -3 Table : -4 Table Value : -5 Key "TeamName" : -6 Table "Teams" 
				{
					std::string sBotFile = lua_tostring(L, -1);
					vecRobots.push_back(make_pair(vecTeamNames.size()-1, sBotFile));
					lua_pop(L, 1); // -1 Key Nil : -2 Table : -3 Table Value : -4 Key "TeamName" : -5 Table "Teams" 
				}				
			}
			lua_pop(L, 1); // -1 Table : -2 Table Value : -3 Key "TeamName" : -4 Table "Teams" 
		}
	}

	// Read in walls
	lua_getglobal(L, "Walls"); // -1 Table "Teams"
	if (lua_istable(L, -1))
	{
		lua_pushnil(L); // -2 Key Nil : -1 Table "Teams"

		while (lua_next(L, -2) != 0) // -1 Table : -2 Key "TeamName" : -3 Table "Teams"
		{			
			if (lua_istable(L, -1))
			{
				lua_gettable(L, -1); // -1 Table : -2 Table Value : -3 Key "TeamName" : -4 Table "Teams" 
				lua_pushnil(L);      // -1 Key Nil : -2 Table : -3 Table Value : -4 Key "TeamName" : -5 Table "Teams" 
				float f[4];
				int i = 0;
				while (lua_next(L, -2) != 0) // -1 Value "BotFile" : -2 Key Nil : -3 Table : -4 Table Value : -5 Key "TeamName" : -6 Table "Teams" 
				{
					f[i] = (float)lua_tonumber(L, -1);
					i++;
					lua_pop(L, 1); // -1 Key Nil : -2 Table : -3 Table Value : -4 Key "TeamName" : -5 Table "Teams" 
				}

				vecWalls.push_back({ f[0], f[1], f[2], f[3] });
			}
			lua_pop(L, 1); // -1 Table : -2 Table Value : -3 Key "TeamName" : -4 Table "Teams" 
		}
	}

	return true;
}

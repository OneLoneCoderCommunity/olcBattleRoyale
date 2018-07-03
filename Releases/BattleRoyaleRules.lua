-- This is a battle plan file. Here you specify the rules and constraints
-- of the battle. During battle you will not have control of these parameters 
-- as they will be set by tournament host

-- DO NOT CHANGE FROM HERE =======================================

-- Enable to stop on robot malfunction and show error message
DebugMode = true

-- Maximum number of robots allowed to enter
MaxRobots = 8

-- Starting Health of robot
MaxRobotHealth = 10

-- Starting energy of robot (in seconds)
MaxRobotEnergy = 30.0

-- Speed of step movement
RobotMoveSpeed = 30.0

-- Duration of step movement
RobotMoveTime = 0.4

-- Speed of turn movement (rad/s)
RobotTurnSpeed = math.pi / 2.0

-- Duration of turn movement
RobotTurnTime = 0.5

-- Maximum Game Time
MaxGameTime = 600.0

-- Gun Cooldown Time
GunCooldown = 0.2

-- Bullet Damage
BulletDamage = 1

-- Bullet Speed
BulletSpeed = 100.0

-- Self Destruct Bullet count
SelfDestructBullets = 10

-- Size of collision circle around bot
RobotHitRadius = 4.0

-- Bots on same team can hurt each other
AllowFriendlyFire = false

-- DO NOT CHANGE ABOVE HERE =======================================

Walls = {
{0, 0, 200, 0},
{0, 0, 0, 200},
{0, 200, 100, 200},
{100, 200, 100, 150},
{100, 150, 150, 100},
{150, 100, 200, 100},
{200, 100, 200, 0}
}



-- Robots can be loaded into teams
Teams = {}
Teams["GoodGuys"] = {"javidbot.lua", "javidbot.lua", "nullbot.lua", "nullbot.lua"}
Teams["BadGuys"] = {"nullbot.lua", "nullbot.lua", "brankbot.lua", "brankbot.lua"}


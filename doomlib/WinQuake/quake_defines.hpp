#ifndef _QUAKE_DEFINES_HPP_
#define _QUAKE_DEFINES_HPP_

#include <cstdint>
#include <istream>
#include <ostream>
#include <string>
#include <type_traits>
#include <algorithm>
#include <string_view>

// try to make this the only macro define we use
#define QUAKE_NAMESPACE quake
#define QUAKE_NS_BEGIN namespace QUAKE_NAMESPACE {
#define QUAKE_NS_END };


QUAKE_NS_BEGIN

	static constexpr int WEAPON_READY = 0;
	static constexpr int WEAPON_ACTIVATING = 1;
	static constexpr int WEAPON_DROPPING = 2;
	static constexpr int WEAPON_FIRING = 3;

	static constexpr float GRENADE_TIMER = 3.0f;
	static constexpr int GRENADE_MINSPEED = 400;
	static constexpr int GRENADE_MAXSPEED = 800;

	// -----------------
	// client/q_shared.h

	// can accelerate and turn
	static constexpr int PM_NORMAL = 0;
	static constexpr int PM_SPECTATOR = 1;
	// no acceleration or turning
	static constexpr int PM_DEAD = 2;
	static constexpr int PM_GIB = 3; // different bounding box 
	static constexpr int PM_FREEZE = 4;

	static constexpr int EV_NONE = 0;
	static constexpr int EV_ITEM_RESPAWN = 1;
	static constexpr int EV_FOOTSTEP = 2;
	static constexpr int EV_FALLSHORT = 3;
	static constexpr int EV_FALL = 4;
	static constexpr int EV_FALLFAR = 5;
	static constexpr int EV_PLAYER_TELEPORT = 6;
	static constexpr int EV_OTHER_TELEPORT = 7;

	//	angle indexes
	static constexpr int PITCH = 0; // up / down 
	static constexpr int YAW = 1; // left / right 
	static constexpr int ROLL = 2; // fall over 

	static constexpr int MAX_STRING_CHARS = 1024; // max length of a string passed to Cmd_TokenizeString 
	static constexpr int MAX_STRING_TOKENS = 80; // max tokens resulting from Cmd_TokenizeString 
	static constexpr int MAX_TOKEN_CHARS = 1024; // max length of an individual token 

	static constexpr int MAX_QPATH = 64; // max length of a quake game pathname 
	static constexpr int MAX_OSPATH = 128; // max length of a filesystem pathname 

											  //	per-level limits
	static constexpr int MAX_CLIENTS = 256; // absolute limit 
	static constexpr int MAX_EDICTS = 1024; // must change protocol to increase more 
	static constexpr int MAX_LIGHTSTYLES = 256;
	static constexpr int MAX_MODELS = 256; // these are sent over the net as bytes 
	static constexpr int MAX_SOUNDS = 256; // so they cannot be blindly increased 
	static constexpr int MAX_IMAGES = 256;
	static constexpr int MAX_ITEMS = 256;
	static constexpr int MAX_GENERAL = (MAX_CLIENTS * 2); // general config strings 

															 //	game print flags
	static constexpr int PRINT_LOW = 0; // pickup messages 
	static constexpr int PRINT_MEDIUM = 1; // death messages 
	static constexpr int PRINT_HIGH = 2; // critical messages 
	static constexpr int PRINT_CHAT = 3; // chat messages 

	static constexpr int ERR_FATAL = 0; // exit the entire game with a popup window 
	static constexpr int ERR_DROP = 1; // print to console and disconnect from game 
	static constexpr int ERR_DISCONNECT = 2; // don't kill server 

	static constexpr int PRINT_ALL = 0;
	static constexpr int PRINT_DEVELOPER = 1; // only print when "developer 1" 
	static constexpr int PRINT_ALERT = 2;

	//	key / value info strings
	static constexpr int MAX_INFO_KEY = 64;
	static constexpr int MAX_INFO_VALUE = 64;
	static constexpr int MAX_INFO_STRING = 512;

	// directory searching
	static constexpr int SFF_ARCH = 0x01;
	static constexpr int SFF_HIDDEN = 0x02;
	static constexpr int SFF_RDONLY = 0x04;
	static constexpr int SFF_SUBDIR = 0x08;
	static constexpr int SFF_SYSTEM = 0x10;

	static constexpr int CVAR_ARCHIVE = 1; // set to cause it to be saved to vars.rc 
	static constexpr int CVAR_USERINFO = 2; // added to userinfo when changed 
	static constexpr int CVAR_SERVERINFO = 4; // added to serverinfo when changed 
	static constexpr int CVAR_NOSET = 8; // don't allow change from console at all, 
											// but can be set from the command line
	static constexpr int CVAR_LATCH = 16; // save changes until server restart 

											 // lower bits are stronger, and will eat weaker brushes completely
	static constexpr int CONTENTS_SOLID = 1; // an eye is never valid in a solid 
	static constexpr int CONTENTS_WINDOW = 2; // translucent, but not watery 
	static constexpr int CONTENTS_AUX = 4;
	static constexpr int CONTENTS_LAVA = 8;
	static constexpr int CONTENTS_SLIME = 16;
	static constexpr int CONTENTS_WATER = 32;
	static constexpr int CONTENTS_MIST = 64;
	static constexpr int LAST_VISIBLE_CONTENTS = 64;

	// remaining contents are non-visible, and don't eat brushes
	static constexpr int CONTENTS_AREAPORTAL = 0x8000;

	static constexpr int CONTENTS_PLAYERCLIP = 0x10000;
	static constexpr int CONTENTS_MONSTERCLIP = 0x20000;

	// currents can be added to any other contents, and may be mixed
	static constexpr int CONTENTS_CURRENT_0 = 0x40000;
	static constexpr int CONTENTS_CURRENT_90 = 0x80000;
	static constexpr int CONTENTS_CURRENT_180 = 0x100000;
	static constexpr int CONTENTS_CURRENT_270 = 0x200000;
	static constexpr int CONTENTS_CURRENT_UP = 0x400000;
	static constexpr int CONTENTS_CURRENT_DOWN = 0x800000;

	static constexpr int CONTENTS_ORIGIN = 0x1000000; // removed before bsping an entity 

	static constexpr int CONTENTS_MONSTER = 0x2000000; // should never be on a brush, only in game 
	static constexpr int CONTENTS_DEADMONSTER = 0x4000000;
	static constexpr int CONTENTS_DETAIL = 0x8000000; // brushes to be added after vis leafs 
	static constexpr int CONTENTS_TRANSLUCENT = 0x10000000; // auto set if any surface has trans 
	static constexpr int CONTENTS_LADDER = 0x20000000;

	static constexpr int SURF_LIGHT = 0x1; // value will hold the light strength 
	static constexpr int SURF_SLICK = 0x2; // effects game physics 

	static constexpr int SURF_SKY = 0x4; // don't draw, but add to skybox 
	static constexpr int SURF_WARP = 0x8; // turbulent water warp 
	static constexpr int SURF_TRANS33 = 0x10;
	static constexpr int SURF_TRANS66 = 0x20;
	static constexpr int SURF_FLOWING = 0x40; // scroll towards angle 
	static constexpr int SURF_NODRAW = 0x80; // don't bother referencing the texture 

												//
												// button bits
												//
	static constexpr int BUTTON_ATTACK = 1;
	static constexpr int BUTTON_USE = 2;
	static constexpr int BUTTON_ANY = 128; // any key whatsoever 

	static constexpr int MAXTOUCH = 32;

	// entity_state_t->effects
	// Effects are things handled on the client side (lights, particles, frame animations)
	// that happen constantly on the given entity.
	// An entity that has effects will be sent to the client
	// even if it has a zero index model.
	static constexpr int EF_ROTATE = 0x00000001; // rotate (bonus items) 
	static constexpr int EF_GIB = 0x00000002; // leave a trail 
	static constexpr int EF_BLASTER = 0x00000008; // redlight + trail 
	static constexpr int EF_ROCKET = 0x00000010; // redlight + trail 
	static constexpr int EF_GRENADE = 0x00000020;
	static constexpr int EF_HYPERBLASTER = 0x00000040;
	static constexpr int EF_BFG = 0x00000080;
	static constexpr int EF_COLOR_SHELL = 0x00000100;
	static constexpr int EF_POWERSCREEN = 0x00000200;
	static constexpr int EF_ANIM01 = 0x00000400; // automatically cycle between frames 0 and 1 at 2 hz 
	static constexpr int EF_ANIM23 = 0x00000800; // automatically cycle between frames 2 and 3 at 2 hz 
	static constexpr int EF_ANIM_ALL = 0x00001000; // automatically cycle through all frames at 2hz 
	static constexpr int EF_ANIM_ALLFAST = 0x00002000; // automatically cycle through all frames at 10hz 
	static constexpr int EF_FLIES = 0x00004000;
	static constexpr int EF_QUAD = 0x00008000;
	static constexpr int EF_PENT = 0x00010000;
	static constexpr int EF_TELEPORTER = 0x00020000; // particle fountain 
	static constexpr int EF_FLAG1 = 0x00040000;
	static constexpr int EF_FLAG2 = 0x00080000;
	// RAFAEL
	static constexpr int EF_IONRIPPER = 0x00100000;
	static constexpr int EF_GREENGIB = 0x00200000;
	static constexpr int EF_BLUEHYPERBLASTER = 0x00400000;
	static constexpr int EF_SPINNINGLIGHTS = 0x00800000;
	static constexpr int EF_PLASMA = 0x01000000;
	static constexpr int EF_TRAP = 0x02000000;

	//ROGUE
	static constexpr int EF_TRACKER = 0x04000000;
	static constexpr int EF_DOUBLE = 0x08000000;
	static constexpr int EF_SPHERETRANS = 0x10000000;
	static constexpr int EF_TAGTRAIL = 0x20000000;
	static constexpr int EF_HALF_DAMAGE = 0x40000000;
	static constexpr int EF_TRACKERTRAIL = 0x80000000;
	//ROGUE

	// entity_state_t->renderfx flags
	static constexpr int RF_MINLIGHT = 1; // allways have some light (viewmodel) 
	static constexpr int RF_VIEWERMODEL = 2; // don't draw through eyes, only mirrors 
	static constexpr int RF_WEAPONMODEL = 4; // only draw through eyes 
	static constexpr int RF_FULLBRIGHT = 8; // allways draw full intensity 
	static constexpr int RF_DEPTHHACK = 16; // for view weapon Z crunching 
	static constexpr int RF_TRANSLUCENT = 32;
	static constexpr int RF_FRAMELERP = 64;
	static constexpr int RF_BEAM = 128;
	static constexpr int RF_CUSTOMSKIN = 256; // skin is an index in image_precache 
	static constexpr int RF_GLOW = 512; // pulse lighting for bonus items 
	static constexpr int RF_SHELL_RED = 1024;
	static constexpr int RF_SHELL_GREEN = 2048;
	static constexpr int RF_SHELL_BLUE = 4096;

	//ROGUE
	static constexpr int RF_IR_VISIBLE = 0x00008000; // 32768 
	static constexpr int RF_SHELL_DOUBLE = 0x00010000; // 65536 
	static constexpr int RF_SHELL_HALF_DAM = 0x00020000;
	static constexpr int RF_USE_DISGUISE = 0x00040000;
	//ROGUE

	// player_state_t->refdef flags
	static constexpr int RDF_UNDERWATER = 1; // warp the screen as apropriate 
	static constexpr int RDF_NOWORLDMODEL = 2; // used for player configuration screen 

												  //ROGUE
	static constexpr int RDF_IRGOGGLES = 4;
	static constexpr int RDF_UVGOGGLES = 8;
	//ROGUE

	// muzzle flashes / player effects
	static constexpr int MZ_BLASTER = 0;
	static constexpr int MZ_MACHINEGUN = 1;
	static constexpr int MZ_SHOTGUN = 2;
	static constexpr int MZ_CHAINGUN1 = 3;
	static constexpr int MZ_CHAINGUN2 = 4;
	static constexpr int MZ_CHAINGUN3 = 5;
	static constexpr int MZ_RAILGUN = 6;
	static constexpr int MZ_ROCKET = 7;
	static constexpr int MZ_GRENADE = 8;
	static constexpr int MZ_LOGIN = 9;
	static constexpr int MZ_LOGOUT = 10;
	static constexpr int MZ_RESPAWN = 11;
	static constexpr int MZ_BFG = 12;
	static constexpr int MZ_SSHOTGUN = 13;
	static constexpr int MZ_HYPERBLASTER = 14;
	static constexpr int MZ_ITEMRESPAWN = 15;
	// RAFAEL
	static constexpr int MZ_IONRIPPER = 16;
	static constexpr int MZ_BLUEHYPERBLASTER = 17;
	static constexpr int MZ_PHALANX = 18;
	static constexpr int MZ_SILENCED = 128; // bit flag ORed with one of the above numbers 

											   //ROGUE
	static constexpr int MZ_ETF_RIFLE = 30;
	static constexpr int MZ_UNUSED = 31;
	static constexpr int MZ_SHOTGUN2 = 32;
	static constexpr int MZ_HEATBEAM = 33;
	static constexpr int MZ_BLASTER2 = 34;
	static constexpr int MZ_TRACKER = 35;
	static constexpr int MZ_NUKE1 = 36;
	static constexpr int MZ_NUKE2 = 37;
	static constexpr int MZ_NUKE4 = 38;
	static constexpr int MZ_NUKE8 = 39;
	//ROGUE

	//
	// monster muzzle flashes
	//
	static constexpr int MZ2_TANK_BLASTER_1 = 1;
	static constexpr int MZ2_TANK_BLASTER_2 = 2;
	static constexpr int MZ2_TANK_BLASTER_3 = 3;
	static constexpr int MZ2_TANK_MACHINEGUN_1 = 4;
	static constexpr int MZ2_TANK_MACHINEGUN_2 = 5;
	static constexpr int MZ2_TANK_MACHINEGUN_3 = 6;
	static constexpr int MZ2_TANK_MACHINEGUN_4 = 7;
	static constexpr int MZ2_TANK_MACHINEGUN_5 = 8;
	static constexpr int MZ2_TANK_MACHINEGUN_6 = 9;
	static constexpr int MZ2_TANK_MACHINEGUN_7 = 10;
	static constexpr int MZ2_TANK_MACHINEGUN_8 = 11;
	static constexpr int MZ2_TANK_MACHINEGUN_9 = 12;
	static constexpr int MZ2_TANK_MACHINEGUN_10 = 13;
	static constexpr int MZ2_TANK_MACHINEGUN_11 = 14;
	static constexpr int MZ2_TANK_MACHINEGUN_12 = 15;
	static constexpr int MZ2_TANK_MACHINEGUN_13 = 16;
	static constexpr int MZ2_TANK_MACHINEGUN_14 = 17;
	static constexpr int MZ2_TANK_MACHINEGUN_15 = 18;
	static constexpr int MZ2_TANK_MACHINEGUN_16 = 19;
	static constexpr int MZ2_TANK_MACHINEGUN_17 = 20;
	static constexpr int MZ2_TANK_MACHINEGUN_18 = 21;
	static constexpr int MZ2_TANK_MACHINEGUN_19 = 22;
	static constexpr int MZ2_TANK_ROCKET_1 = 23;
	static constexpr int MZ2_TANK_ROCKET_2 = 24;
	static constexpr int MZ2_TANK_ROCKET_3 = 25;

	static constexpr int MZ2_INFANTRY_MACHINEGUN_1 = 26;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_2 = 27;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_3 = 28;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_4 = 29;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_5 = 30;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_6 = 31;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_7 = 32;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_8 = 33;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_9 = 34;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_10 = 35;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_11 = 36;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_12 = 37;
	static constexpr int MZ2_INFANTRY_MACHINEGUN_13 = 38;

	static constexpr int MZ2_SOLDIER_BLASTER_1 = 39;
	static constexpr int MZ2_SOLDIER_BLASTER_2 = 40;
	static constexpr int MZ2_SOLDIER_SHOTGUN_1 = 41;
	static constexpr int MZ2_SOLDIER_SHOTGUN_2 = 42;
	static constexpr int MZ2_SOLDIER_MACHINEGUN_1 = 43;
	static constexpr int MZ2_SOLDIER_MACHINEGUN_2 = 44;

	static constexpr int MZ2_GUNNER_MACHINEGUN_1 = 45;
	static constexpr int MZ2_GUNNER_MACHINEGUN_2 = 46;
	static constexpr int MZ2_GUNNER_MACHINEGUN_3 = 47;
	static constexpr int MZ2_GUNNER_MACHINEGUN_4 = 48;
	static constexpr int MZ2_GUNNER_MACHINEGUN_5 = 49;
	static constexpr int MZ2_GUNNER_MACHINEGUN_6 = 50;
	static constexpr int MZ2_GUNNER_MACHINEGUN_7 = 51;
	static constexpr int MZ2_GUNNER_MACHINEGUN_8 = 52;
	static constexpr int MZ2_GUNNER_GRENADE_1 = 53;
	static constexpr int MZ2_GUNNER_GRENADE_2 = 54;
	static constexpr int MZ2_GUNNER_GRENADE_3 = 55;
	static constexpr int MZ2_GUNNER_GRENADE_4 = 56;

	static constexpr int MZ2_CHICK_ROCKET_1 = 57;

	static constexpr int MZ2_FLYER_BLASTER_1 = 58;
	static constexpr int MZ2_FLYER_BLASTER_2 = 59;

	static constexpr int MZ2_MEDIC_BLASTER_1 = 60;

	static constexpr int MZ2_GLADIATOR_RAILGUN_1 = 61;

	static constexpr int MZ2_HOVER_BLASTER_1 = 62;

	static constexpr int MZ2_ACTOR_MACHINEGUN_1 = 63;

	static constexpr int MZ2_SUPERTANK_MACHINEGUN_1 = 64;
	static constexpr int MZ2_SUPERTANK_MACHINEGUN_2 = 65;
	static constexpr int MZ2_SUPERTANK_MACHINEGUN_3 = 66;
	static constexpr int MZ2_SUPERTANK_MACHINEGUN_4 = 67;
	static constexpr int MZ2_SUPERTANK_MACHINEGUN_5 = 68;
	static constexpr int MZ2_SUPERTANK_MACHINEGUN_6 = 69;
	static constexpr int MZ2_SUPERTANK_ROCKET_1 = 70;
	static constexpr int MZ2_SUPERTANK_ROCKET_2 = 71;
	static constexpr int MZ2_SUPERTANK_ROCKET_3 = 72;

	static constexpr int MZ2_BOSS2_MACHINEGUN_L1 = 73;
	static constexpr int MZ2_BOSS2_MACHINEGUN_L2 = 74;
	static constexpr int MZ2_BOSS2_MACHINEGUN_L3 = 75;
	static constexpr int MZ2_BOSS2_MACHINEGUN_L4 = 76;
	static constexpr int MZ2_BOSS2_MACHINEGUN_L5 = 77;
	static constexpr int MZ2_BOSS2_ROCKET_1 = 78;
	static constexpr int MZ2_BOSS2_ROCKET_2 = 79;
	static constexpr int MZ2_BOSS2_ROCKET_3 = 80;
	static constexpr int MZ2_BOSS2_ROCKET_4 = 81;

	static constexpr int MZ2_FLOAT_BLASTER_1 = 82;

	static constexpr int MZ2_SOLDIER_BLASTER_3 = 83;
	static constexpr int MZ2_SOLDIER_SHOTGUN_3 = 84;
	static constexpr int MZ2_SOLDIER_MACHINEGUN_3 = 85;
	static constexpr int MZ2_SOLDIER_BLASTER_4 = 86;
	static constexpr int MZ2_SOLDIER_SHOTGUN_4 = 87;
	static constexpr int MZ2_SOLDIER_MACHINEGUN_4 = 88;
	static constexpr int MZ2_SOLDIER_BLASTER_5 = 89;
	static constexpr int MZ2_SOLDIER_SHOTGUN_5 = 90;
	static constexpr int MZ2_SOLDIER_MACHINEGUN_5 = 91;
	static constexpr int MZ2_SOLDIER_BLASTER_6 = 92;
	static constexpr int MZ2_SOLDIER_SHOTGUN_6 = 93;
	static constexpr int MZ2_SOLDIER_MACHINEGUN_6 = 94;
	static constexpr int MZ2_SOLDIER_BLASTER_7 = 95;
	static constexpr int MZ2_SOLDIER_SHOTGUN_7 = 96;
	static constexpr int MZ2_SOLDIER_MACHINEGUN_7 = 97;
	static constexpr int MZ2_SOLDIER_BLASTER_8 = 98;
	static constexpr int MZ2_SOLDIER_SHOTGUN_8 = 99;
	static constexpr int MZ2_SOLDIER_MACHINEGUN_8 = 100;

	// --- Xian shit below ---
	static constexpr int MZ2_MAKRON_BFG = 101;
	static constexpr int MZ2_MAKRON_BLASTER_1 = 102;
	static constexpr int MZ2_MAKRON_BLASTER_2 = 103;
	static constexpr int MZ2_MAKRON_BLASTER_3 = 104;
	static constexpr int MZ2_MAKRON_BLASTER_4 = 105;
	static constexpr int MZ2_MAKRON_BLASTER_5 = 106;
	static constexpr int MZ2_MAKRON_BLASTER_6 = 107;
	static constexpr int MZ2_MAKRON_BLASTER_7 = 108;
	static constexpr int MZ2_MAKRON_BLASTER_8 = 109;
	static constexpr int MZ2_MAKRON_BLASTER_9 = 110;
	static constexpr int MZ2_MAKRON_BLASTER_10 = 111;
	static constexpr int MZ2_MAKRON_BLASTER_11 = 112;
	static constexpr int MZ2_MAKRON_BLASTER_12 = 113;
	static constexpr int MZ2_MAKRON_BLASTER_13 = 114;
	static constexpr int MZ2_MAKRON_BLASTER_14 = 115;
	static constexpr int MZ2_MAKRON_BLASTER_15 = 116;
	static constexpr int MZ2_MAKRON_BLASTER_16 = 117;
	static constexpr int MZ2_MAKRON_BLASTER_17 = 118;
	static constexpr int MZ2_MAKRON_RAILGUN_1 = 119;
	static constexpr int MZ2_JORG_MACHINEGUN_L1 = 120;
	static constexpr int MZ2_JORG_MACHINEGUN_L2 = 121;
	static constexpr int MZ2_JORG_MACHINEGUN_L3 = 122;
	static constexpr int MZ2_JORG_MACHINEGUN_L4 = 123;
	static constexpr int MZ2_JORG_MACHINEGUN_L5 = 124;
	static constexpr int MZ2_JORG_MACHINEGUN_L6 = 125;
	static constexpr int MZ2_JORG_MACHINEGUN_R1 = 126;
	static constexpr int MZ2_JORG_MACHINEGUN_R2 = 127;
	static constexpr int MZ2_JORG_MACHINEGUN_R3 = 128;
	static constexpr int MZ2_JORG_MACHINEGUN_R4 = 129;
	static constexpr int MZ2_JORG_MACHINEGUN_R5 = 130;
	static constexpr int MZ2_JORG_MACHINEGUN_R6 = 131;
	static constexpr int MZ2_JORG_BFG_1 = 132;
	static constexpr int MZ2_BOSS2_MACHINEGUN_R1 = 133;
	static constexpr int MZ2_BOSS2_MACHINEGUN_R2 = 134;
	static constexpr int MZ2_BOSS2_MACHINEGUN_R3 = 135;
	static constexpr int MZ2_BOSS2_MACHINEGUN_R4 = 136;
	static constexpr int MZ2_BOSS2_MACHINEGUN_R5 = 137;

	//ROGUE
	static constexpr int MZ2_CARRIER_MACHINEGUN_L1 = 138;
	static constexpr int MZ2_CARRIER_MACHINEGUN_R1 = 139;
	static constexpr int MZ2_CARRIER_GRENADE = 140;
	static constexpr int MZ2_TURRET_MACHINEGUN = 141;
	static constexpr int MZ2_TURRET_ROCKET = 142;
	static constexpr int MZ2_TURRET_BLASTER = 143;
	static constexpr int MZ2_STALKER_BLASTER = 144;
	static constexpr int MZ2_DAEDALUS_BLASTER = 145;
	static constexpr int MZ2_MEDIC_BLASTER_2 = 146;
	static constexpr int MZ2_CARRIER_RAILGUN = 147;
	static constexpr int MZ2_WIDOW_DISRUPTOR = 148;
	static constexpr int MZ2_WIDOW_BLASTER = 149;
	static constexpr int MZ2_WIDOW_RAIL = 150;
	static constexpr int MZ2_WIDOW_PLASMABEAM = 151; // PMM - not used 
	static constexpr int MZ2_CARRIER_MACHINEGUN_L2 = 152;
	static constexpr int MZ2_CARRIER_MACHINEGUN_R2 = 153;
	static constexpr int MZ2_WIDOW_RAIL_LEFT = 154;
	static constexpr int MZ2_WIDOW_RAIL_RIGHT = 155;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP1 = 156;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP2 = 157;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP3 = 158;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP4 = 159;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP5 = 160;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP6 = 161;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP7 = 162;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP8 = 163;
	static constexpr int MZ2_WIDOW_BLASTER_SWEEP9 = 164;
	static constexpr int MZ2_WIDOW_BLASTER_100 = 165;
	static constexpr int MZ2_WIDOW_BLASTER_90 = 166;
	static constexpr int MZ2_WIDOW_BLASTER_80 = 167;
	static constexpr int MZ2_WIDOW_BLASTER_70 = 168;
	static constexpr int MZ2_WIDOW_BLASTER_60 = 169;
	static constexpr int MZ2_WIDOW_BLASTER_50 = 170;
	static constexpr int MZ2_WIDOW_BLASTER_40 = 171;
	static constexpr int MZ2_WIDOW_BLASTER_30 = 172;
	static constexpr int MZ2_WIDOW_BLASTER_20 = 173;
	static constexpr int MZ2_WIDOW_BLASTER_10 = 174;
	static constexpr int MZ2_WIDOW_BLASTER_0 = 175;
	static constexpr int MZ2_WIDOW_BLASTER_10L = 176;
	static constexpr int MZ2_WIDOW_BLASTER_20L = 177;
	static constexpr int MZ2_WIDOW_BLASTER_30L = 178;
	static constexpr int MZ2_WIDOW_BLASTER_40L = 179;
	static constexpr int MZ2_WIDOW_BLASTER_50L = 180;
	static constexpr int MZ2_WIDOW_BLASTER_60L = 181;
	static constexpr int MZ2_WIDOW_BLASTER_70L = 182;
	static constexpr int MZ2_WIDOW_RUN_1 = 183;
	static constexpr int MZ2_WIDOW_RUN_2 = 184;
	static constexpr int MZ2_WIDOW_RUN_3 = 185;
	static constexpr int MZ2_WIDOW_RUN_4 = 186;
	static constexpr int MZ2_WIDOW_RUN_5 = 187;
	static constexpr int MZ2_WIDOW_RUN_6 = 188;
	static constexpr int MZ2_WIDOW_RUN_7 = 189;
	static constexpr int MZ2_WIDOW_RUN_8 = 190;
	static constexpr int MZ2_CARRIER_ROCKET_1 = 191;
	static constexpr int MZ2_CARRIER_ROCKET_2 = 192;
	static constexpr int MZ2_CARRIER_ROCKET_3 = 193;
	static constexpr int MZ2_CARRIER_ROCKET_4 = 194;
	static constexpr int MZ2_WIDOW2_BEAMER_1 = 195;
	static constexpr int MZ2_WIDOW2_BEAMER_2 = 196;
	static constexpr int MZ2_WIDOW2_BEAMER_3 = 197;
	static constexpr int MZ2_WIDOW2_BEAMER_4 = 198;
	static constexpr int MZ2_WIDOW2_BEAMER_5 = 199;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_1 = 200;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_2 = 201;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_3 = 202;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_4 = 203;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_5 = 204;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_6 = 205;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_7 = 206;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_8 = 207;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_9 = 208;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_10 = 209;
	static constexpr int MZ2_WIDOW2_BEAM_SWEEP_11 = 210;

	static constexpr int SPLASH_UNKNOWN = 0;
	static constexpr int SPLASH_SPARKS = 1;
	static constexpr int SPLASH_BLUE_WATER = 2;
	static constexpr int SPLASH_BROWN_WATER = 3;
	static constexpr int SPLASH_SLIME = 4;
	static constexpr int SPLASH_LAVA = 5;
	static constexpr int SPLASH_BLOOD = 6;

	//	   sound channels
	//	   channel 0 never willingly overrides
	//	   other channels (1-7) allways override a playing sound on that channel
	static constexpr int CHAN_AUTO = 0;
	static constexpr int CHAN_WEAPON = 1;
	static constexpr int CHAN_VOICE = 2;
	static constexpr int CHAN_ITEM = 3;
	static constexpr int CHAN_BODY = 4;
	//	   modifier flags
	static constexpr int CHAN_NO_PHS_ADD = 8;
	// send to all clients, not just ones in PHS (ATTN 0 will also do this)
	static constexpr int CHAN_RELIABLE = 16; // send by reliable message, not datagram 

												//	   sound attenuation values
	static constexpr int ATTN_NONE = 0; // full volume the entire level 
	static constexpr int ATTN_NORM = 1;
	static constexpr int ATTN_IDLE = 2;
	static constexpr int ATTN_STATIC = 3; // diminish very rapidly with distance 

											 //	   player_state->stats[] indexes
	static constexpr int STAT_HEALTH_ICON = 0;
	static constexpr int STAT_HEALTH = 1;
	static constexpr int STAT_AMMO_ICON = 2;
	static constexpr int STAT_AMMO = 3;
	static constexpr int STAT_ARMOR_ICON = 4;
	static constexpr int STAT_ARMOR = 5;
	static constexpr int STAT_SELECTED_ICON = 6;
	static constexpr int STAT_PICKUP_ICON = 7;
	static constexpr int STAT_PICKUP_STRING = 8;
	static constexpr int STAT_TIMER_ICON = 9;
	static constexpr int STAT_TIMER = 10;
	static constexpr int STAT_HELPICON = 11;
	static constexpr int STAT_SELECTED_ITEM = 12;
	static constexpr int STAT_LAYOUTS = 13;
	static constexpr int STAT_FRAGS = 14;
	static constexpr int STAT_FLASHES = 15; // cleared each frame, 1 = health, 2 = armor 
	static constexpr int STAT_CHASE = 16;
	static constexpr int STAT_SPECTATOR = 17;

	static constexpr int MAX_STATS = 32;

	//	   dmflags->value flags
	static constexpr int DF_NO_HEALTH = 0x00000001; // 1 
	static constexpr int DF_NO_ITEMS = 0x00000002; // 2 
	static constexpr int DF_WEAPONS_STAY = 0x00000004; // 4 
	static constexpr int DF_NO_FALLING = 0x00000008; // 8 
	static constexpr int DF_INSTANT_ITEMS = 0x00000010; // 16 
	static constexpr int DF_SAME_LEVEL = 0x00000020; // 32 
	static constexpr int DF_SKINTEAMS = 0x00000040; // 64 
	static constexpr int DF_MODELTEAMS = 0x00000080; // 128 
	static constexpr int DF_NO_FRIENDLY_FIRE = 0x00000100; // 256 
	static constexpr int DF_SPAWN_FARTHEST = 0x00000200; // 512 
	static constexpr int DF_FORCE_RESPAWN = 0x00000400; // 1024 
	static constexpr int DF_NO_ARMOR = 0x00000800; // 2048 
	static constexpr int DF_ALLOW_EXIT = 0x00001000; // 4096 
	static constexpr int DF_INFINITE_AMMO = 0x00002000; // 8192 
	static constexpr int DF_QUAD_DROP = 0x00004000; // 16384 
	static constexpr int DF_FIXED_FOV = 0x00008000; // 32768 

													   //	   RAFAEL
	static constexpr int DF_QUADFIRE_DROP = 0x00010000; // 65536 

														   //	  ROGUE
	static constexpr int DF_NO_MINES = 0x00020000;
	static constexpr int DF_NO_STACK_DOUBLE = 0x00040000;
	static constexpr int DF_NO_NUKES = 0x00080000;
	static constexpr int DF_NO_SPHERES = 0x00100000;
	//	  ROGUE

	//
	//	config strings are a general means of communication from
	//	the server to all connected clients.
	//	Each config string can be at most MAX_QPATH characters.
	//
	static constexpr int CS_NAME = 0;
	static constexpr int CS_CDTRACK = 1;
	static constexpr int CS_SKY = 2;
	static constexpr int CS_SKYAXIS = 3; // %f %f %f format 
	static constexpr int CS_SKYROTATE = 4;
	static constexpr int CS_STATUSBAR = 5; // display program string 

	static constexpr int CS_AIRACCEL = 29; // air acceleration control 
	static constexpr int CS_MAXCLIENTS = 30;
	static constexpr int CS_MAPCHECKSUM = 31; // for catching cheater maps 

	static constexpr int CS_MODELS = 32;
	static constexpr int CS_SOUNDS = (CS_MODELS + MAX_MODELS);
	static constexpr int CS_IMAGES = (CS_SOUNDS + MAX_SOUNDS);
	static constexpr int CS_LIGHTS = (CS_IMAGES + MAX_IMAGES);
	static constexpr int CS_ITEMS = (CS_LIGHTS + MAX_LIGHTSTYLES);
	static constexpr int CS_PLAYERSKINS = (CS_ITEMS + MAX_ITEMS);
	static constexpr int CS_GENERAL = (CS_PLAYERSKINS + MAX_CLIENTS);
	static constexpr int MAX_CONFIGSTRINGS = (CS_GENERAL + MAX_GENERAL);

	static constexpr int HEALTH_IGNORE_MAX = 1;
	static constexpr int HEALTH_TIMED = 2;

	// gi.BoxEdicts() can return a list of either solid or trigger entities
	// FIXME: eliminate AREA_ distinction?
	static constexpr int AREA_SOLID = 1;
	static constexpr int AREA_TRIGGERS = 2;

	static constexpr int TE_GUNSHOT = 0;
	static constexpr int TE_BLOOD = 1;
	static constexpr int TE_BLASTER = 2;
	static constexpr int TE_RAILTRAIL = 3;
	static constexpr int TE_SHOTGUN = 4;
	static constexpr int TE_EXPLOSION1 = 5;
	static constexpr int TE_EXPLOSION2 = 6;
	static constexpr int TE_ROCKET_EXPLOSION = 7;
	static constexpr int TE_GRENADE_EXPLOSION = 8;
	static constexpr int TE_SPARKS = 9;
	static constexpr int TE_SPLASH = 10;
	static constexpr int TE_BUBBLETRAIL = 11;
	static constexpr int TE_SCREEN_SPARKS = 12;
	static constexpr int TE_SHIELD_SPARKS = 13;
	static constexpr int TE_BULLET_SPARKS = 14;
	static constexpr int TE_LASER_SPARKS = 15;
	static constexpr int TE_PARASITE_ATTACK = 16;
	static constexpr int TE_ROCKET_EXPLOSION_WATER = 17;
	static constexpr int TE_GRENADE_EXPLOSION_WATER = 18;
	static constexpr int TE_MEDIC_CABLE_ATTACK = 19;
	static constexpr int TE_BFG_EXPLOSION = 20;
	static constexpr int TE_BFG_BIGEXPLOSION = 21;
	static constexpr int TE_BOSSTPORT = 22; // used as '22' in a map, so DON'T RENUMBER!!! 
	static constexpr int TE_BFG_LASER = 23;
	static constexpr int TE_GRAPPLE_CABLE = 24;
	static constexpr int TE_WELDING_SPARKS = 25;
	static constexpr int TE_GREENBLOOD = 26;
	static constexpr int TE_BLUEHYPERBLASTER = 27;
	static constexpr int TE_PLASMA_EXPLOSION = 28;
	static constexpr int TE_TUNNEL_SPARKS = 29;
	//ROGUE 
	static constexpr int TE_BLASTER2 = 30;
	static constexpr int TE_RAILTRAIL2 = 31;
	static constexpr int TE_FLAME = 32;
	static constexpr int TE_LIGHTNING = 33;
	static constexpr int TE_DEBUGTRAIL = 34;
	static constexpr int TE_PLAIN_EXPLOSION = 35;
	static constexpr int TE_FLASHLIGHT = 36;
	static constexpr int TE_FORCEWALL = 37;
	static constexpr int TE_HEATBEAM = 38;
	static constexpr int TE_MONSTER_HEATBEAM = 39;
	static constexpr int TE_STEAM = 40;
	static constexpr int TE_BUBBLETRAIL2 = 41;
	static constexpr int TE_MOREBLOOD = 42;
	static constexpr int TE_HEATBEAM_SPARKS = 43;
	static constexpr int TE_HEATBEAM_STEAM = 44;
	static constexpr int TE_CHAINFIST_SMOKE = 45;
	static constexpr int TE_ELECTRIC_SPARKS = 46;
	static constexpr int TE_TRACKER_EXPLOSION = 47;
	static constexpr int TE_TELEPORT_EFFECT = 48;
	static constexpr int TE_DBALL_GOAL = 49;
	static constexpr int TE_WIDOWBEAMOUT = 50;
	static constexpr int TE_NUKEBLAST = 51;
	static constexpr int TE_WIDOWSPLASH = 52;
	static constexpr int TE_EXPLOSION1_BIG = 53;
	static constexpr int TE_EXPLOSION1_NP = 54;
	static constexpr int TE_FLECHETTE = 55;

	//	content masks
	static constexpr int MASK_ALL = (-1);
	static constexpr int MASK_SOLID = (CONTENTS_SOLID | CONTENTS_WINDOW);
	static constexpr int MASK_PLAYERSOLID = (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER);
	static constexpr int MASK_DEADSOLID = (CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_WINDOW);
	static constexpr int MASK_MONSTERSOLID = (CONTENTS_SOLID | CONTENTS_MONSTERCLIP | CONTENTS_WINDOW | CONTENTS_MONSTER);
	static constexpr int MASK_WATER = (CONTENTS_WATER | CONTENTS_LAVA | CONTENTS_SLIME);
	static constexpr int MASK_OPAQUE = (CONTENTS_SOLID | CONTENTS_SLIME | CONTENTS_LAVA);
	static constexpr int MASK_SHOT = (CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEADMONSTER);
	static constexpr int MASK_CURRENT =
		(CONTENTS_CURRENT_0
			| CONTENTS_CURRENT_90
			| CONTENTS_CURRENT_180
			| CONTENTS_CURRENT_270
			| CONTENTS_CURRENT_UP
			| CONTENTS_CURRENT_DOWN);

	// item spawnflags
	static constexpr int ITEM_TRIGGER_SPAWN = 0x00000001;
	static constexpr int ITEM_NO_TOUCH = 0x00000002;
	// 6 bits reserved for editor flags
	// 8 bits used as power cube id bits for coop games
	static constexpr int DROPPED_ITEM = 0x00010000;
	static constexpr int DROPPED_PLAYER_ITEM = 0x00020000;
	static constexpr int ITEM_TARGETS_USED = 0x00040000;

	// (machen nur GL)
	static constexpr int VIDREF_GL = 1;
	static constexpr int VIDREF_SOFT = 2;
	static constexpr int VIDREF_OTHER = 3;

	// --------------
	// game/g_local.h

	static constexpr int FFL_SPAWNTEMP = 1;
	static constexpr int FFL_NOSPAWN = 2;

	// enum fieldtype_t
	static constexpr int F_INT = 0;
	static constexpr int F_FLOAT = 1;
	static constexpr int F_LSTRING = 2; // string on disk, pointer in memory, TAG_LEVEL
	static constexpr int F_GSTRING = 3; // string on disk, pointer in memory, TAG_GAME
	static constexpr int F_VECTOR = 4;
	static constexpr int F_ANGLEHACK = 5;
	static constexpr int F_EDICT = 6; // index on disk, pointer in memory
	static constexpr int F_ITEM = 7; // index on disk, pointer in memory
	static constexpr int F_CLIENT = 8; // index on disk, pointer in memory
	static constexpr int F_FUNCTION = 9;
	static constexpr int F_MMOVE = 10;
	static constexpr int F_IGNORE = 11;

	static constexpr int DEFAULT_BULLET_HSPREAD = 300;
	static constexpr int DEFAULT_BULLET_VSPREAD = 500;
	static constexpr int DEFAULT_SHOTGUN_HSPREAD = 1000;
	static constexpr int DEFAULT_SHOTGUN_VSPREAD = 500;
	static constexpr int DEFAULT_DEATHMATCH_SHOTGUN_COUNT = 12;
	static constexpr int DEFAULT_SHOTGUN_COUNT = 12;
	static constexpr int DEFAULT_SSHOTGUN_COUNT = 20;

	static constexpr int ANIM_BASIC = 0; // stand / run 
	static constexpr int ANIM_WAVE = 1;
	static constexpr int ANIM_JUMP = 2;
	static constexpr int ANIM_PAIN = 3;
	static constexpr int ANIM_ATTACK = 4;
	static constexpr int ANIM_DEATH = 5;
	static constexpr int ANIM_REVERSE = 6;

	static constexpr int AMMO_BULLETS = 0;
	static constexpr int AMMO_SHELLS = 1;
	static constexpr int AMMO_ROCKETS = 2;
	static constexpr int AMMO_GRENADES = 3;
	static constexpr int AMMO_CELLS = 4;
	static constexpr int AMMO_SLUGS = 5;

	//	view pitching times
	static constexpr float DAMAGE_TIME = 0.5f;
	static constexpr float FALL_TIME = 0.3f;

	//	damage flags
	static constexpr int DAMAGE_RADIUS = 0x00000001; // damage was indirect 
	static constexpr int DAMAGE_NO_ARMOR = 0x00000002; // armour does not protect from this damage 
	static constexpr int DAMAGE_ENERGY = 0x00000004; // damage is from an energy based weapon 
	static constexpr int DAMAGE_NO_KNOCKBACK = 0x00000008; // do not affect velocity, just view angles 
	static constexpr int DAMAGE_BULLET = 0x00000010; // damage is from a bullet (used for ricochets) 
	static constexpr int DAMAGE_NO_PROTECTION = 0x00000020;
	// armor, shields, invulnerability, and godmode have no effect

	static constexpr int DAMAGE_NO = 0;
	static constexpr int DAMAGE_YES = 1; // will take damage if hit 
	static constexpr int DAMAGE_AIM = 2; // auto targeting recognizes this 

											//	means of death
	static constexpr int MOD_UNKNOWN = 0;
	static constexpr int MOD_BLASTER = 1;
	static constexpr int MOD_SHOTGUN = 2;
	static constexpr int MOD_SSHOTGUN = 3;
	static constexpr int MOD_MACHINEGUN = 4;
	static constexpr int MOD_CHAINGUN = 5;
	static constexpr int MOD_GRENADE = 6;
	static constexpr int MOD_G_SPLASH = 7;
	static constexpr int MOD_ROCKET = 8;
	static constexpr int MOD_R_SPLASH = 9;
	static constexpr int MOD_HYPERBLASTER = 10;
	static constexpr int MOD_RAILGUN = 11;
	static constexpr int MOD_BFG_LASER = 12;
	static constexpr int MOD_BFG_BLAST = 13;
	static constexpr int MOD_BFG_EFFECT = 14;
	static constexpr int MOD_HANDGRENADE = 15;
	static constexpr int MOD_HG_SPLASH = 16;
	static constexpr int MOD_WATER = 17;
	static constexpr int MOD_SLIME = 18;
	static constexpr int MOD_LAVA = 19;
	static constexpr int MOD_CRUSH = 20;
	static constexpr int MOD_TELEFRAG = 21;
	static constexpr int MOD_FALLING = 22;
	static constexpr int MOD_SUICIDE = 23;
	static constexpr int MOD_HELD_GRENADE = 24;
	static constexpr int MOD_EXPLOSIVE = 25;
	static constexpr int MOD_BARREL = 26;
	static constexpr int MOD_BOMB = 27;
	static constexpr int MOD_EXIT = 28;
	static constexpr int MOD_SPLASH = 29;
	static constexpr int MOD_TARGET_LASER = 30;
	static constexpr int MOD_TRIGGER_HURT = 31;
	static constexpr int MOD_HIT = 32;
	static constexpr int MOD_TARGET_BLASTER = 33;
	static constexpr int MOD_FRIENDLY_FIRE = 0x8000000;

	//	edict->spawnflags
	//	these are set with checkboxes on each entity in the map editor
	static constexpr int SPAWNFLAG_NOT_EASY = 0x00000100;
	static constexpr int SPAWNFLAG_NOT_MEDIUM = 0x00000200;
	static constexpr int SPAWNFLAG_NOT_HARD = 0x00000400;
	static constexpr int SPAWNFLAG_NOT_DEATHMATCH = 0x00000800;
	static constexpr int SPAWNFLAG_NOT_COOP = 0x00001000;

	//	edict->flags
	static constexpr int FL_FLY = 0x00000001;
	static constexpr int FL_SWIM = 0x00000002; // implied immunity to drowining 
	static constexpr int FL_IMMUNE_LASER = 0x00000004;
	static constexpr int FL_INWATER = 0x00000008;
	static constexpr int FL_GODMODE = 0x00000010;
	static constexpr int FL_NOTARGET = 0x00000020;
	static constexpr int FL_IMMUNE_SLIME = 0x00000040;
	static constexpr int FL_IMMUNE_LAVA = 0x00000080;
	static constexpr int FL_PARTIALGROUND = 0x00000100; // not all corners are valid 
	static constexpr int FL_WATERJUMP = 0x00000200; // player jumping out of water 
	static constexpr int FL_TEAMSLAVE = 0x00000400; // not the first on the team 
	static constexpr int FL_NO_KNOCKBACK = 0x00000800;
	static constexpr int FL_POWER_ARMOR = 0x00001000; // power armor (if any) is active 
	static constexpr int FL_RESPAWN = 0x80000000; // used for item respawning 

	static constexpr float FRAMETIME = 0.1f;

	//	memory tags to allow dynamic memory to be cleaned up
	static constexpr int TAG_GAME = 765; // clear when unloading the dll 
	static constexpr int TAG_LEVEL = 766; // clear when loading a new level 

	static constexpr int MELEE_DISTANCE = 80;

	static constexpr int BODY_QUEUE_SIZE = 8;

	//	deadflag
	static constexpr int DEAD_NO = 0;
	static constexpr int DEAD_DYING = 1;
	static constexpr int DEAD_DEAD = 2;
	static constexpr int DEAD_RESPAWNABLE = 3;

	//	range
	static constexpr int RANGE_MELEE = 0;
	static constexpr int RANGE_NEAR = 1;
	static constexpr int RANGE_MID = 2;
	static constexpr int RANGE_FAR = 3;

	//	gib types
	static constexpr int GIB_ORGANIC = 0;
	static constexpr int GIB_METALLIC = 1;

	//	monster ai flags
	static constexpr int AI_STAND_GROUND = 0x00000001;
	static constexpr int AI_TEMP_STAND_GROUND = 0x00000002;
	static constexpr int AI_SOUND_TARGET = 0x00000004;
	static constexpr int AI_LOST_SIGHT = 0x00000008;
	static constexpr int AI_PURSUIT_LAST_SEEN = 0x00000010;
	static constexpr int AI_PURSUE_NEXT = 0x00000020;
	static constexpr int AI_PURSUE_TEMP = 0x00000040;
	static constexpr int AI_HOLD_FRAME = 0x00000080;
	static constexpr int AI_GOOD_GUY = 0x00000100;
	static constexpr int AI_BRUTAL = 0x00000200;
	static constexpr int AI_NOSTEP = 0x00000400;
	static constexpr int AI_DUCKED = 0x00000800;
	static constexpr int AI_COMBAT_POINT = 0x00001000;
	static constexpr int AI_MEDIC = 0x00002000;
	static constexpr int AI_RESURRECTING = 0x00004000;

	//	monster attack state
	static constexpr int AS_STRAIGHT = 1;
	static constexpr int AS_SLIDING = 2;
	static constexpr int AS_MELEE = 3;
	static constexpr int AS_MISSILE = 4;

	//	 armor types
	static constexpr int ARMOR_NONE = 0;
	static constexpr int ARMOR_JACKET = 1;
	static constexpr int ARMOR_COMBAT = 2;
	static constexpr int ARMOR_BODY = 3;
	static constexpr int ARMOR_SHARD = 4;

	//	 power armor types
	static constexpr int POWER_ARMOR_NONE = 0;
	static constexpr int POWER_ARMOR_SCREEN = 1;
	static constexpr int POWER_ARMOR_SHIELD = 2;

	//	 handedness values
	static constexpr int RIGHT_HANDED = 0;
	static constexpr int LEFT_HANDED = 1;
	static constexpr int CENTER_HANDED = 2;

	//	 game.serverflags values
	static constexpr int SFL_CROSS_TRIGGER_1 = 0x00000001;
	static constexpr int SFL_CROSS_TRIGGER_2 = 0x00000002;
	static constexpr int SFL_CROSS_TRIGGER_3 = 0x00000004;
	static constexpr int SFL_CROSS_TRIGGER_4 = 0x00000008;
	static constexpr int SFL_CROSS_TRIGGER_5 = 0x00000010;
	static constexpr int SFL_CROSS_TRIGGER_6 = 0x00000020;
	static constexpr int SFL_CROSS_TRIGGER_7 = 0x00000040;
	static constexpr int SFL_CROSS_TRIGGER_8 = 0x00000080;
	static constexpr int SFL_CROSS_TRIGGER_MASK = 0x000000ff;

	//	 noise types for PlayerNoise
	static constexpr int PNOISE_SELF = 0;
	static constexpr int PNOISE_WEAPON = 1;
	static constexpr int PNOISE_IMPACT = 2;

	//	gitem_t->flags
	static constexpr int IT_WEAPON = 1; // use makes active weapon 
	static constexpr int IT_AMMO = 2;
	static constexpr int IT_ARMOR = 4;
	static constexpr int IT_STAY_COOP = 8;
	static constexpr int IT_KEY = 16;
	static constexpr int IT_POWERUP = 32;

	//	gitem_t->weapmodel for weapons indicates model index
	static constexpr int WEAP_BLASTER = 1;
	static constexpr int WEAP_SHOTGUN = 2;
	static constexpr int WEAP_SUPERSHOTGUN = 3;
	static constexpr int WEAP_MACHINEGUN = 4;
	static constexpr int WEAP_CHAINGUN = 5;
	static constexpr int WEAP_GRENADES = 6;
	static constexpr int WEAP_GRENADELAUNCHER = 7;
	static constexpr int WEAP_ROCKETLAUNCHER = 8;
	static constexpr int WEAP_HYPERBLASTER = 9;
	static constexpr int WEAP_RAILGUN = 10;
	static constexpr int WEAP_BFG = 11;

	//	edict->movetype values
	static constexpr int MOVETYPE_NONE = 0; // never moves 
	static constexpr int MOVETYPE_NOCLIP = 1; // origin and angles change with no interaction 
	static constexpr int MOVETYPE_PUSH = 2; // no clip to world, push on box contact 
	static constexpr int MOVETYPE_STOP = 3; // no clip to world, stops on box contact 

	static constexpr int MOVETYPE_WALK = 4; // gravity 
	static constexpr int MOVETYPE_STEP = 5; // gravity, special edge handling 
	static constexpr int MOVETYPE_FLY = 6;
	static constexpr int MOVETYPE_TOSS = 7; // gravity 
	static constexpr int MOVETYPE_FLYMISSILE = 8; // extra size to monsters 
	static constexpr int MOVETYPE_BOUNCE = 9;

	static constexpr int MULTICAST_ALL = 0;
	static constexpr int MULTICAST_PHS = 1;
	static constexpr int MULTICAST_PVS = 2;
	static constexpr int MULTICAST_ALL_R = 3;
	static constexpr int MULTICAST_PHS_R = 4;
	static constexpr int MULTICAST_PVS_R = 5;

	// -------------
	// client/game.h

	static constexpr int SOLID_NOT = 0; // no interaction with other objects
	static constexpr int SOLID_TRIGGER = 1; // only touch when inside, after moving
	static constexpr int SOLID_BBOX = 2; // touch on edge
	static constexpr int SOLID_BSP = 3; // bsp clip, touch on edge

	static constexpr int GAME_API_VERSION = 3;

	//	   edict->svflags
	static constexpr int SVF_NOCLIENT = 0x00000001; // don't send entity to clients, even if it has effects 
	static constexpr int SVF_DEADMONSTER = 0x00000002; // treat as CONTENTS_DEADMONSTER for collision 
	static constexpr int SVF_MONSTER = 0x00000004; // treat as CONTENTS_MONSTER for collision 

	static constexpr int MAX_ENT_CLUSTERS = 16;

	static constexpr int sv_stopspeed = 100;
	static constexpr int sv_friction = 6;
	static constexpr int sv_waterfriction = 1;

	static constexpr int PLAT_LOW_TRIGGER = 1;

	static constexpr int STATE_TOP = 0;
	static constexpr int STATE_BOTTOM = 1;
	static constexpr int STATE_UP = 2;
	static constexpr int STATE_DOWN = 3;

	static constexpr int DOOR_START_OPEN = 1;
	static constexpr int DOOR_REVERSE = 2;
	static constexpr int DOOR_CRUSHER = 4;
	static constexpr int DOOR_NOMONSTER = 8;
	static constexpr int DOOR_TOGGLE = 32;
	static constexpr int DOOR_X_AXIS = 64;
	static constexpr int DOOR_Y_AXIS = 128;

	// R E N D E R E R 
	////////////////////
	static constexpr int MAX_DLIGHTS = 32;
	static constexpr int MAX_ENTITIES = 128;
	static constexpr int MAX_PARTICLES = 4096;

	// gl_model.h
	static constexpr int SURF_PLANEBACK = 2;
	static constexpr int SURF_DRAWSKY = 4;
	static constexpr int SURF_DRAWTURB = 0x10;
	static constexpr int SURF_DRAWBACKGROUND = 0x40;
	static constexpr int SURF_UNDERWATER = 0x80;

	static constexpr float POWERSUIT_SCALE = 4.0f;

	static constexpr int SHELL_RED_COLOR = 0xF2;
	static constexpr int SHELL_GREEN_COLOR = 0xD0;
	static constexpr int SHELL_BLUE_COLOR = 0xF3;

	static constexpr int SHELL_RG_COLOR = 0xDC;

	static constexpr int SHELL_RB_COLOR = 0x68; //0x86
	static constexpr int SHELL_BG_COLOR = 0x78;

	// ROGUE
	static constexpr int SHELL_DOUBLE_COLOR = 0xDF; // 223
	static constexpr int SHELL_HALF_DAM_COLOR = 0x90;
	static constexpr int SHELL_CYAN_COLOR = 0x72;

	// ---------
	// qcommon.h

	static constexpr int svc_bad = 0;

	// these ops are known to the game dll
	// protocol bytes that can be directly added to messages

	static constexpr int svc_muzzleflash = 1;
	static constexpr int svc_muzzleflash2 = 2;
	static constexpr int svc_temp_entity = 3;
	static constexpr int svc_layout = 4;
	static constexpr int svc_inventory = 5;

	// the rest are private to the client and server
	static constexpr int svc_nop = 6;
	static constexpr int svc_disconnect = 7;
	static constexpr int svc_reconnect = 8;
	static constexpr int svc_sound = 9; // <see code> 
	static constexpr int svc_print = 10; // [byte] id [string] null terminated string 
	static constexpr int svc_stufftext = 11;
	// [string] stuffed into client's console buffer, should be \n terminated
	static constexpr int svc_serverdata = 12; // [long] protocol ... 
	static constexpr int svc_configstring = 13; // [short] [string] 
	static constexpr int svc_spawnbaseline = 14;
	static constexpr int svc_centerprint = 15; // [string] to put in center of the screen 
	static constexpr int svc_download = 16; // [short] size [size bytes] 
	static constexpr int svc_playerinfo = 17; // variable 
	static constexpr int svc_packetentities = 18; // [...] 
	static constexpr int svc_deltapacketentities = 19; // [...] 
	static constexpr int svc_frame = 20;

	static constexpr int NUMVERTEXNORMALS = 162;
	static constexpr int PROTOCOL_VERSION = 34;
	static constexpr int PORT_MASTER = 27900;
	static constexpr int PORT_CLIENT = 27901;
	static constexpr int PORT_SERVER = 27910;
	static constexpr int PORT_ANY = -1;

	static constexpr int PS_M_TYPE = (1 << 0);
	static constexpr int PS_M_ORIGIN = (1 << 1);
	static constexpr int PS_M_VELOCITY = (1 << 2);
	static constexpr int PS_M_TIME = (1 << 3);
	static constexpr int PS_M_FLAGS = (1 << 4);
	static constexpr int PS_M_GRAVITY = (1 << 5);
	static constexpr int PS_M_DELTA_ANGLES = (1 << 6);

	static constexpr int UPDATE_BACKUP = 16; // copies of entity_state_t to keep buffered 
												// must be power of two
	static constexpr int UPDATE_MASK = (UPDATE_BACKUP - 1);

	static constexpr int PS_VIEWOFFSET = (1 << 7);
	static constexpr int PS_VIEWANGLES = (1 << 8);
	static constexpr int PS_KICKANGLES = (1 << 9);
	static constexpr int PS_BLEND = (1 << 10);
	static constexpr int PS_FOV = (1 << 11);
	static constexpr int PS_WEAPONINDEX = (1 << 12);
	static constexpr int PS_WEAPONFRAME = (1 << 13);
	static constexpr int PS_RDFLAGS = (1 << 14);

	static constexpr int CM_ANGLE1 = (1 << 0);
	static constexpr int CM_ANGLE2 = (1 << 1);
	static constexpr int CM_ANGLE3 = (1 << 2);
	static constexpr int CM_FORWARD = (1 << 3);
	static constexpr int CM_SIDE = (1 << 4);
	static constexpr int CM_UP = (1 << 5);
	static constexpr int CM_BUTTONS = (1 << 6);
	static constexpr int CM_IMPULSE = (1 << 7);

	// try to pack the common update flags into the first byte
	static constexpr int U_ORIGIN1 = (1 << 0);
	static constexpr int U_ORIGIN2 = (1 << 1);
	static constexpr int U_ANGLE2 = (1 << 2);
	static constexpr int U_ANGLE3 = (1 << 3);
	static constexpr int U_FRAME8 = (1 << 4); // frame is a byte 
	static constexpr int U_EVENT = (1 << 5);
	static constexpr int U_REMOVE = (1 << 6); // REMOVE this entity, don't add it 
	static constexpr int U_MOREBITS1 = (1 << 7); // read one additional byte 

													// second byte
	static constexpr int U_NUMBER16 = (1 << 8); // NUMBER8 is implicit if not set 
	static constexpr int U_ORIGIN3 = (1 << 9);
	static constexpr int U_ANGLE1 = (1 << 10);
	static constexpr int U_MODEL = (1 << 11);
	static constexpr int U_RENDERFX8 = (1 << 12); // fullbright, etc 
	static constexpr int U_EFFECTS8 = (1 << 14); // autorotate, trails, etc 
	static constexpr int U_MOREBITS2 = (1 << 15); // read one additional byte 

													 // third byte
	static constexpr int U_SKIN8 = (1 << 16);
	static constexpr int U_FRAME16 = (1 << 17); // frame is a short 
	static constexpr int U_RENDERFX16 = (1 << 18); // 8 + 16 = 32 
	static constexpr int U_EFFECTS16 = (1 << 19); // 8 + 16 = 32 
	static constexpr int U_MODEL2 = (1 << 20); // weapons, flags, etc 
	static constexpr int U_MODEL3 = (1 << 21);
	static constexpr int U_MODEL4 = (1 << 22);
	static constexpr int U_MOREBITS3 = (1 << 23); // read one additional byte 

													 // fourth byte
	static constexpr int U_OLDORIGIN = (1 << 24); // FIXME: get rid of this 
	static constexpr int U_SKIN16 = (1 << 25);
	static constexpr int U_SOUND = (1 << 26);
	static constexpr int U_SOLID = (1 << 27);

	static constexpr int SHELL_WHITE_COLOR = 0xD7;

	static constexpr int MAX_TRIANGLES = 4096;
	static constexpr int MAX_VERTS = 2048;
	static constexpr int MAX_FRAMES = 512;
	static constexpr int MAX_MD2SKINS = 32;
	static constexpr int MAX_SKINNAME = 64;

	static constexpr int MAXLIGHTMAPS = 4;
	static constexpr int MIPLEVELS = 4;

	static constexpr int clc_bad = 0;
	static constexpr int clc_nop = 1;
	static constexpr int clc_move = 2; // [[usercmd_t]
	static constexpr int clc_userinfo = 3; // [[userinfo string]
	static constexpr int clc_stringcmd = 4; // [string] message

	static constexpr int NS_CLIENT = 0;
	static constexpr int NS_SERVER = 1;

	static constexpr int NA_LOOPBACK = 0;
	static constexpr int NA_BROADCAST = 1;
	static constexpr int NA_IP = 2;
	static constexpr int NA_IPX = 3;
	static constexpr int NA_BROADCAST_IPX = 4;

	static constexpr int SND_VOLUME = (1 << 0); // a byte 
	static constexpr int SND_ATTENUATION = (1 << 1); // a byte 
	static constexpr int SND_POS = (1 << 2); // three coordinates 
	static constexpr int SND_ENT = (1 << 3); // a short 0-2: channel, 3-12: entity 
	static constexpr int SND_OFFSET = (1 << 4); // a byte, msec offset from frame start 

	static constexpr float DEFAULT_SOUND_PACKET_VOLUME = 1.0f;
	static constexpr float DEFAULT_SOUND_PACKET_ATTENUATION = 1.0f;

	// --------
	// client.h
	static constexpr int MAX_PARSE_ENTITIES = 1024;
	static constexpr int MAX_CLIENTWEAPONMODELS = 20;

	static constexpr  int CMD_BACKUP = 64; // allow a lot of command backups for very fast systems	

	static constexpr int ca_uninitialized = 0;
	static constexpr int ca_disconnected = 1;
	static constexpr int ca_connecting = 2;
	static constexpr int ca_connected = 3;
	static constexpr int ca_active = 4;

	static constexpr int MAX_ALIAS_NAME = 32;
	static constexpr int MAX_NUM_ARGVS = 50;

	static constexpr int MAX_MSGLEN = 1400;

	// ---------
	// console.h
	static constexpr int NUM_CON_TIMES = 4;
	static constexpr int CON_TEXTSIZE = 32768;

	static constexpr int BSPVERSION = 38;

	// --------
	// qfiles.h 

	// upper design bounds
	// leaffaces, leafbrushes, planes, and verts are still bounded by
	// 16 bit short limits
	static constexpr int MAX_MAP_MODELS = 1024;
	static constexpr int MAX_MAP_BRUSHES = 8192;
	static constexpr int MAX_MAP_ENTITIES = 2048;
	static constexpr int MAX_MAP_ENTSTRING = 0x40000;
	static constexpr int MAX_MAP_TEXINFO = 8192;

	static constexpr int MAX_MAP_AREAS = 256;
	static constexpr int MAX_MAP_AREAPORTALS = 1024;
	static constexpr int MAX_MAP_PLANES = 65536;
	static constexpr int MAX_MAP_NODES = 65536;
	static constexpr int MAX_MAP_BRUSHSIDES = 65536;
	static constexpr int MAX_MAP_LEAFS = 65536;
	static constexpr int MAX_MAP_VERTS = 65536;
	static constexpr int MAX_MAP_FACES = 65536;
	static constexpr int MAX_MAP_LEAFFACES = 65536;
	static constexpr int MAX_MAP_LEAFBRUSHES = 65536;
	static constexpr int MAX_MAP_PORTALS = 65536;
	static constexpr int MAX_MAP_EDGES = 128000;
	static constexpr int MAX_MAP_SURFEDGES = 256000;
	static constexpr int MAX_MAP_LIGHTING = 0x200000;
	static constexpr int MAX_MAP_VISIBILITY = 0x100000;

	// key / value pair sizes
	static constexpr int MAX_KEY = 32;
	static constexpr int MAX_VALUE = 1024;

	// 0-2 are axial planes
	static constexpr int PLANE_X = 0;
	static constexpr int PLANE_Y = 1;
	static constexpr int PLANE_Z = 2;

	// 3-5 are non-axial planes snapped to the nearest
	static constexpr int PLANE_ANYX = 3;
	static constexpr int PLANE_ANYY = 4;
	static constexpr int PLANE_ANYZ = 5;

	static constexpr int LUMP_ENTITIES = 0;
	static constexpr int LUMP_PLANES = 1;
	static constexpr int LUMP_VERTEXES = 2;
	static constexpr int LUMP_VISIBILITY = 3;
	static constexpr int LUMP_NODES = 4;
	static constexpr int LUMP_TEXINFO = 5;
	static constexpr int LUMP_FACES = 6;
	static constexpr int LUMP_LIGHTING = 7;
	static constexpr int LUMP_LEAFS = 8;
	static constexpr int LUMP_LEAFFACES = 9;
	static constexpr int LUMP_LEAFBRUSHES = 10;
	static constexpr int LUMP_EDGES = 11;
	static constexpr int LUMP_SURFEDGES = 12;
	static constexpr int LUMP_MODELS = 13;
	static constexpr int LUMP_BRUSHES = 14;
	static constexpr int LUMP_BRUSHSIDES = 15;
	static constexpr int LUMP_POP = 16;
	static constexpr int LUMP_AREAS = 17;
	static constexpr int LUMP_AREAPORTALS = 18;
	static constexpr int HEADER_LUMPS = 19;

	static constexpr int DTRIVERTX_V0 = 0;
	static constexpr int DTRIVERTX_V1 = 1;
	static constexpr int DTRIVERTX_V2 = 2;
	static constexpr int DTRIVERTX_LNI = 3;
	static constexpr int DTRIVERTX_SIZE = 4;

	static constexpr int ALIAS_VERSION = 8;
	static constexpr const char* GAMEVERSION = "baseq2";
	static constexpr int API_VERSION = 3; // ref_library (refexport_t)

	static constexpr int DVIS_PVS = 0;
	static constexpr int DVIS_PHS = 1;

	// ----------------
	// client/keydest_t
	static constexpr int key_game = 0;
	static constexpr int key_console = 1;
	static constexpr int key_message = 2;
	static constexpr int key_menu = 3;

	// ---------------
	// server/server.h
	static constexpr int cs_free = 0; // can be reused for a new connection
	static constexpr int cs_zombie = 1; // client has been disconnected, but don't reuse
										   // connection for a couple seconds
	static constexpr int cs_connected = 2; // has been assigned to a client_t, but not in game yet
	static constexpr int cs_spawned = 3;

	static constexpr int MAX_CHALLENGES = 1024;

	static constexpr int ss_dead = 0; // no map loaded
	static constexpr int ss_loading = 1; // spawning level edicts
	static constexpr int ss_game = 2; // actively running
	static constexpr int ss_cinematic = 3;
	static constexpr int ss_demo = 4;
	static constexpr int ss_pic = 5;

	static constexpr int SV_OUTPUTBUF_LENGTH = (MAX_MSGLEN - 16);
	static constexpr int RD_NONE = 0;
	static constexpr int RD_CLIENT = 1;
	static constexpr int RD_PACKET = 2;

	static constexpr int RATE_MESSAGES = 10;

	static constexpr int LATENCY_COUNTS = 16;

	static constexpr int MAXCMDLINE = 256;

	static constexpr int MAX_MASTERS = 8;

	//server/sv_world.h
	static constexpr int AREA_DEPTH = 4;
	static constexpr int AREA_NODES = 32;

	static constexpr int EXEC_NOW = 0;
	static constexpr int EXEC_INSERT = 1;
	static constexpr int EXEC_APPEND = 2;

	//client/qmenu.h
	static constexpr int MAXMENUITEMS = 64;

	static constexpr int MTYPE_SLIDER = 0;
	static constexpr int MTYPE_LIST = 1;
	static constexpr int MTYPE_ACTION = 2;
	static constexpr int MTYPE_SPINCONTROL = 3;
	static constexpr int MTYPE_SEPARATOR = 4;
	static constexpr int MTYPE_FIELD = 5;

	static constexpr int K_TAB = 9;
	static constexpr int K_ENTER = 13;
	static constexpr int K_ESCAPE = 27;
	static constexpr int K_SPACE = 32;

	// normal keys should be passed as lowercased ascii

	static constexpr int K_BACKSPACE = 127;
	static constexpr int K_UPARROW = 128;
	static constexpr int K_DOWNARROW = 129;
	static constexpr int K_LEFTARROW = 130;
	static constexpr int K_RIGHTARROW = 131;

	static constexpr int QMF_LEFT_JUSTIFY = 0x00000001;
	static constexpr int QMF_GRAYED = 0x00000002;
	static constexpr int QMF_NUMBERSONLY = 0x00000004;

	static constexpr int RCOLUMN_OFFSET = 16;
	static constexpr int LCOLUMN_OFFSET = -16;

	static constexpr int MAX_DISPLAYNAME = 16;
	static constexpr int MAX_PLAYERMODELS = 1024;

	static constexpr int MAX_LOCAL_SERVERS = 8;
	static constexpr const char* NO_SERVER_STRING = "<no server>";
	static constexpr int NUM_ADDRESSBOOK_ENTRIES = 9;

	static constexpr int STEPSIZE = 18;


	static constexpr float MOVE_STOP_EPSILON = 0.1f;

	static constexpr float MIN_STEP_NORMAL = 0.7f; // can't step up onto very steep slopes


													  // used by filefinders in Sys
	static constexpr int FILEISREADABLE = 1;

	static constexpr int FILEISWRITABLE = 2;

	static constexpr int FILEISFILE = 4;

	static constexpr int FILEISDIRECTORY = 8;

	// datentyp konstanten
	// groesse in bytes
	// just to get this all to work we check eveything... EVEYTHING

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__) || \
	(defined(_MSC_VER) && (defined(_M_PPC))) 	/* ms visual studio defines xbox */
	// It's a big-endian target architecture
	static constexpr bool LITTLE_ENDIAN = false;
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) || \
	(defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64) || defined(_M_ARM))) 	/* ms visual studio defines */
	// It's a little-endian target architecture
	static constexpr bool LITTLE_ENDIAN = true;
#else
#error "I don't know what architecture this is!"
#endif

	static constexpr int SIZE_OF_SHORT = 2;

	static constexpr int SIZE_OF_INT = 4;

	static constexpr int SIZE_OF_LONG = 8;

	static constexpr int SIZE_OF_FLOAT = 4;

	static constexpr int SIZE_OF_DOUBLE = 8;
#if 0
	static constexpr int SIZE_OF_SHORT = sizeof(short);

	static constexpr int SIZE_OF_INT = sizeof(int);

	static constexpr int SIZE_OF_LONG = sizeof(long);

	static constexpr int SIZE_OF_FLOAT = sizeof(float);

	static constexpr int SIZE_OF_DOUBLE = sizeof(double);

#endif

QUAKE_NS_END




#endif

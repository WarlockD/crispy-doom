#ifndef __QUAKE_GLOBALS_HPP_
#define __QUAKE_GLOBALS_HPP_

#include "quake_defines.hpp"
#include "../quake/quakedef.h"

QUAKE_NS_BEGIN



struct Globals {
	// start - chase.c
	cvar_t	chase_back = { "chase_back", "100" };
	cvar_t	chase_up = { "chase_up", "16" };
	cvar_t	chase_right = { "chase_right", "0" };
	cvar_t	chase_active = { "chase_active", "0" };

	vec3_t	chase_pos;
	vec3_t	chase_angles;

	vec3_t	chase_dest;
	vec3_t	chase_dest_angles;
	void Chase_Init(void);
	void Chase_Reset(void);
	void TraceLine(vec3_t& start, vec3_t& end, vec3_t& impact);
	void Chase_Update(void);
	// end - chase.c
	// start - cl_demo.c
	void CL_FinishTimeDemo(void);
	void CL_StopPlayback(void);
	void CL_WriteDemoMessage(void);
	int CL_GetMessage(void);
	void CL_Stop_f(void);
	void CL_Record_f(void);
	void CL_PlayDemo_f(void);
	void CL_FinishTimeDemo(void);
	void CL_TimeDemo_f(void);
	// end - cl_demo.c
	// start - cl_input.c
	kbutton_t	in_mlook, in_klook;
	kbutton_t	in_left, in_right, in_forward, in_back;
	kbutton_t	in_lookup, in_lookdown, in_moveleft, in_moveright;
	kbutton_t	in_strafe, in_speed, in_use, in_jump, in_attack;
	kbutton_t	in_up, in_down;

	int			in_impulse;
	void KeyDown(kbutton_t *b);
	void KeyUp(kbutton_t *b);
	void IN_KLookDown(void) { KeyDown(&in_klook); }
	void IN_KLookUp(void) { KeyUp(&in_klook); }
	void IN_MLookDown(void) { KeyDown(&in_mlook); }
	void IN_MLookUp(void) {
		KeyUp(&in_mlook);
		if (!(in_mlook.state & 1) && lookspring.value)
			V_StartPitchDrift();
	}
	void IN_UpDown(void) { KeyDown(&in_up); }
	void IN_UpUp(void) { KeyUp(&in_up); }
	void IN_DownDown(void) { KeyDown(&in_down); }
	void IN_DownUp(void) { KeyUp(&in_down); }
	void IN_LeftDown(void) { KeyDown(&in_left); }
	void IN_LeftUp(void) { KeyUp(&in_left); }
	void IN_RightDown(void) { KeyDown(&in_right); }
	void IN_RightUp(void) { KeyUp(&in_right); }
	void IN_ForwardDown(void) { KeyDown(&in_forward); }
	void IN_ForwardUp(void) { KeyUp(&in_forward); }
	void IN_BackDown(void) { KeyDown(&in_back); }
	void IN_BackUp(void) { KeyUp(&in_back); }
	void IN_LookupDown(void) { KeyDown(&in_lookup); }
	void IN_LookupUp(void) { KeyUp(&in_lookup); }
	void IN_LookdownDown(void) { KeyDown(&in_lookdown); }
	void IN_LookdownUp(void) { KeyUp(&in_lookdown); }
	void IN_MoveleftDown(void) { KeyDown(&in_moveleft); }
	void IN_MoveleftUp(void) { KeyUp(&in_moveleft); }
	void IN_MoverightDown(void) { KeyDown(&in_moveright); }
	void IN_MoverightUp(void) { KeyUp(&in_moveright); }

	void IN_SpeedDown(void) { KeyDown(&in_speed); }
	void IN_SpeedUp(void) { KeyUp(&in_speed); }
	void IN_StrafeDown(void) { KeyDown(&in_strafe); }
	void IN_StrafeUp(void) { KeyUp(&in_strafe); }

	void IN_AttackDown(void) { KeyDown(&in_attack); }
	void IN_AttackUp(void) { KeyUp(&in_attack); }

	void IN_UseDown(void) { KeyDown(&in_use); }
	void IN_UseUp(void) { KeyUp(&in_use); }
	void IN_JumpDown(void) { KeyDown(&in_jump); }
	void IN_JumpUp(void) { KeyUp(&in_jump); }

	void IN_Impulse(void) { in_impulse = Q_atoi(Cmd_Argv(1)); }
	// this is stupid, why not flags or a mask?:P
	/*
	===============
	CL_KeyState

	Returns 0.25 if a key was pressed and released during the frame,
	0.5 if it was pressed and held
	0 if held then released, and
	1.0 if held for the entire time
	===============
	*/
	float CL_KeyState(kbutton_t *key);
	cvar_t	cl_upspeed = { "cl_upspeed","200" };
	cvar_t	cl_forwardspeed = { "cl_forwardspeed","200", true };
	cvar_t	cl_backspeed = { "cl_backspeed","200", true };
	cvar_t	cl_sidespeed = { "cl_sidespeed","350" };

	cvar_t	cl_movespeedkey = { "cl_movespeedkey","2.0" };

	cvar_t	cl_yawspeed = { "cl_yawspeed","140" };
	cvar_t	cl_pitchspeed = { "cl_pitchspeed","150" };

	cvar_t	cl_anglespeedkey = { "cl_anglespeedkey","1.5" };
	void CL_AdjustAngles(void);
	void CL_BaseMove(usercmd_t *cmd);
	void CL_SendMove(usercmd_t *cmd);
	void CL_InitInput(void);
	// end - cl_input.c
	// start -cl_main.c
	// these two are not intended to be set directly
	cvar_t	cl_name = { "_cl_name", "player", true };
	cvar_t	cl_color = { "_cl_color", "0", true };

	cvar_t	cl_shownet = { "cl_shownet","0" };	// can be 0, 1, or 2
	cvar_t	cl_nolerp = { "cl_nolerp","0" };

	cvar_t	lookspring = { "lookspring","0", true };
	cvar_t	lookstrafe = { "lookstrafe","0", true };
	cvar_t	sensitivity = { "sensitivity","3", true };

	cvar_t	m_pitch = { "m_pitch","0.022", true };
	cvar_t	m_yaw = { "m_yaw","0.022", true };
	cvar_t	m_forward = { "m_forward","1", true };
	cvar_t	m_side = { "m_side","0.8", true };
	client_static_t	cls;
	client_state_t	cl;
	// FIXME: put these on hunk?
	efrag_t			cl_efrags[MAX_EFRAGS];
	entity_t		cl_entities[MAX_EDICTS];
	entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
	lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
	dlight_t		cl_dlights[MAX_DLIGHTS];

	int				cl_numvisedicts;
	entity_t		*cl_visedicts[MAX_VISEDICTS];
	void CL_ClearState(void);
	void CL_Disconnect(void);
	void CL_Disconnect_f(void);
	void CL_EstablishConnection(char *host);
	void CL_SignonReply(void);
	void CL_NextDemo(void);
	void CL_PrintEntities_f(void);
	void SetPal(int i);
	dlight_t *CL_AllocDlight(int key);
	void CL_DecayLights(void);
	float	CL_LerpPoint(void);
	void CL_RelinkEntities(void);
	int CL_ReadFromServer(void);
	void CL_SendCmd(void);
	void CL_Init(void);
	// end - cl_main.c
	// start - cl_parse.c

	static constexpr const char *svc_strings[] =
	{
		"svc_bad",
		"svc_nop",
		"svc_disconnect",
		"svc_updatestat",
		"svc_version",		// [long] server version
		"svc_setview",		// [short] entity number
		"svc_sound",			// <see code>
		"svc_time",			// [float] server time
		"svc_print",			// [string] null terminated string
		"svc_stufftext",		// [string] stuffed into client's console buffer
								// the string should be \n terminated
		"svc_setangle",		// [vec3] set the view angle to this absolute value

		"svc_serverinfo",		// [long] version
		// [string] signon string
		// [string]..[0]model cache [string]...[0]sounds cache
		// [string]..[0]item cache
		"svc_lightstyle",		// [byte] [string]
		"svc_updatename",		// [byte] [string]
		"svc_updatefrags",	// [byte] [short]
		"svc_clientdata",		// <shortbits + data>
		"svc_stopsound",		// <see code>
		"svc_updatecolors",	// [byte] [byte]
		"svc_particle",		// [vec3] <variable>
		"svc_damage",			// [byte] impact [byte] blood [vec3] from

		"svc_spawnstatic",
		"OBSOLETE svc_spawnbinary",
		"svc_spawnbaseline",

		"svc_temp_entity",		// <variable>
		"svc_setpause",
		"svc_signonnum",
		"svc_centerprint",
		"svc_killedmonster",
		"svc_foundsecret",
		"svc_spawnstaticsound",
		"svc_intermission",
		"svc_finale",			// [string] music [string] text
		"svc_cdtrack",			// [byte] track [byte] looptrack
		"svc_sellscreen",
		"svc_cutscene"
	};
	entity_t	*CL_EntityNum(int num);
	void CL_ParseStartSoundPacket(void);
	void CL_KeepaliveMessage(void);
	void CL_ParseServerInfo(void);
	void CL_ParseUpdate(int bits);
	void CL_ParseBaseline(entity_t *ent);
	void CL_ParseClientdata(int bits);
	void CL_NewTranslation(int slot);
	void CL_ParseStatic(void);
	void CL_ParseStaticSound(void);
	void CL_ParseServerMessage(void);
	// end - cl_parse.c
	// start - cl_tent.c
	int			num_temp_entities;
	entity_t	cl_temp_entities[MAX_TEMP_ENTITIES];
	beam_t		cl_beams[MAX_BEAMS];

	sfx_t			*cl_sfx_wizhit;
	sfx_t			*cl_sfx_knighthit;
	sfx_t			*cl_sfx_tink1;
	sfx_t			*cl_sfx_ric1;
	sfx_t			*cl_sfx_ric2;
	sfx_t			*cl_sfx_ric3;
	sfx_t			*cl_sfx_r_exp3;
#ifdef QUAKE2
	sfx_t			*cl_sfx_imp;
	sfx_t			*cl_sfx_rail;
#endif
	void CL_InitTEnts(void);
	void CL_ParseBeam(model_t *m);
	void CL_ParseTEnt(void);
	entity_t *CL_NewTempEntity(void);

};

extern Globals global;

QUAKE_NS_END


#endif
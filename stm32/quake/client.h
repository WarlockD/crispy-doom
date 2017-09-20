/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// client.h

typedef struct
{
	vec3_t	viewangles;

// intended velocities
	float	forwardmove;
	float	sidemove;
	float	upmove;
#ifdef QUAKE2
	byte	lightlevel;
#endif
} usercmd_t;

typedef struct
{
	int		length;
	char	map[MAX_STYLESTRING];
} lightstyle_t;

typedef struct
{
	char	name[MAX_SCOREBOARDNAME];
	float	entertime;
	int		frags;
	int		colors;			// two 4 bit fields
	byte	translations[VID_GRADES*256];
} scoreboard_t;

typedef struct
{
	int		destcolor[3];
	int		percent;		// 0-256
} cshift_t;

#define	CSHIFT_CONTENTS	0
#define	CSHIFT_DAMAGE	1
#define	CSHIFT_BONUS	2
#define	CSHIFT_POWERUP	3
#define	NUM_CSHIFTS		4

#define	NAME_LENGTH	64


//
// client_state_t should hold all pieces of the client state
//

#define	SIGNONS		4			// signon messages to receive before connected

#define	MAX_DLIGHTS		32
typedef struct
{
	vec3_t	origin;
	float	radius;
	float	die;				// stop lighting after this time
	float	decay;				// drop this each second
	float	minlight;			// don't add when contributing less
	int		key;
#ifdef QUAKE2
	qboolean	dark;			// subtracts light instead of adding
#endif
} dlight_t;


#define	MAX_BEAMS	24
typedef struct
{
	int		entity;
	struct model_s	*model;
	float	endtime;
	vec3_t	start, end;
} beam_t;

#define	MAX_EFRAGS		640

#define	MAX_MAPSTRING	2048
#define	MAX_DEMOS		8
#define	MAX_DEMONAME	16

typedef enum {
ca_dedicated, 		// a dedicated server with no ability to start a client
ca_disconnected, 	// full screen console with no connection
ca_connected		// valid netcon, talking to a server
} cactive_t;

//
// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
typedef struct
{
	cactive_t	state;

// personalization data sent to server	
	char		mapstring[MAX_QPATH];
	char		spawnparms[MAX_MAPSTRING];	// to restart a level

// demo loop control
	int			demonum;		// -1 = don't play demos
	char		demos[MAX_DEMOS][MAX_DEMONAME];		// when not playing

// demo recording info must be here, because record is started before
// entering a map (and clearing client_state_t)
	qboolean	demorecording;
	qboolean	demoplayback;
	qboolean	timedemo;
	int			forcetrack;			// -1 = use normal cd track
	FILE		*demofile;
	int			td_lastframe;		// to meter out one message a frame
	int			td_startframe;		// host_framecount at start
	float		td_starttime;		// realtime at second frame of timedemo


// connection information
	int			signon;			// 0 to SIGNONS
	struct qsocket_s	*netcon;
	sizebuf_t	message;		// writing buffer to send to server
	
} client_static_t;

EXTERN  client_static_t	cls;

//
// the client_state_t structure is wiped completely at every
// server signon
//
typedef struct
{
	int			movemessages;	// since connecting to this server
								// throw out the first couple, so the player
								// doesn't accidentally do something the 
								// first frame
	usercmd_t	cmd;			// last command sent to the server

// information for local display
	int			stats[MAX_CL_STATS];	// health, etc
	int			items;			// inventory bit flags
	float	item_gettime[32];	// cl.time of aquiring item, for blinking
	float		faceanimtime;	// use anim frame if cl.time < this

	cshift_t	cshifts[NUM_CSHIFTS];	// color shifts for damage, powerups
	cshift_t	prev_cshifts[NUM_CSHIFTS];	// and content types

// the client maintains its own idea of view angles, which are
// sent to the server each frame.  The server sets punchangle when
// the view is temporarliy offset, and an angle reset commands at the start
// of each level and after teleporting.
	vec3_t		mviewangles[2];	// during demo playback viewangles is lerped
								// between these
	vec3_t		viewangles;
	
	vec3_t		mvelocity[2];	// update by server, used for lean+bob
								// (0 is newest)
	vec3_t		velocity;		// lerped between mvelocity[0] and [1]

	vec3_t		punchangle;		// temporary offset
	
// pitch drifting vars
	float		idealpitch;
	float		pitchvel;
	qboolean	nodrift;
	float		driftmove;
	double		laststop;

	float		viewheight;
	float		crouch;			// local amount for smoothing stepups

	qboolean	paused;			// send over by server
	qboolean	onground;
	qboolean	inwater;
	
	int			intermission;	// don't change view angle, full screen, etc
	int			completed_time;	// latched at intermission start
	
	double		mtime[2];		// the timestamp of last two messages	
	double		time;			// clients view of time, should be between
								// servertime and oldservertime to generate
								// a lerp point for other data
	double		oldtime;		// previous cl.time, time-oldtime is used
								// to decay light values and smooth step ups
	

	float		last_received_message;	// (realtime) for net trouble icon

//
// information that is static for the entire time connected to a server
//
	struct model_s		*model_precache[MAX_MODELS];
	struct sfx_s		*sound_precache[MAX_SOUNDS];

	char		levelname[40];	// for display on solo scoreboard
	int			viewentity;		// cl_entitites[cl.viewentity] = player
	int			maxclients;
	int			gametype;

// refresh related state
	struct model_s	*worldmodel;	// cl_entitites[0].model
	struct efrag_s	*free_efrags;
	int			num_entities;	// held in cl_entities array
	int			num_statics;	// held in cl_staticentities array
	entity_t	viewent;			// the gun model

	int			cdtrack, looptrack;	// cd audio

// frag scoreboard
	scoreboard_t	*scores;		// [cl.maxclients]

#ifdef QUAKE2
// light level at player's position including dlights
// this is sent back to the server each frame
// architectually ugly but it works
	int			light_level;
#endif
} client_state_t;


//
// cvars
//
EXTERN 	cvar_t	cl_name;
EXTERN 	cvar_t	cl_color;

EXTERN 	cvar_t	cl_upspeed;
EXTERN 	cvar_t	cl_forwardspeed;
EXTERN 	cvar_t	cl_backspeed;
EXTERN 	cvar_t	cl_sidespeed;

EXTERN 	cvar_t	cl_movespeedkey;

EXTERN 	cvar_t	cl_yawspeed;
EXTERN 	cvar_t	cl_pitchspeed;

EXTERN 	cvar_t	cl_anglespeedkey;

EXTERN 	cvar_t	cl_autofire;

EXTERN 	cvar_t	cl_shownet;
EXTERN 	cvar_t	cl_nolerp;

EXTERN 	cvar_t	cl_pitchdriftspeed;
EXTERN 	cvar_t	lookspring;
EXTERN 	cvar_t	lookstrafe;
EXTERN 	cvar_t	sensitivity;

EXTERN 	cvar_t	m_pitch;
EXTERN 	cvar_t	m_yaw;
EXTERN 	cvar_t	m_forward;
EXTERN 	cvar_t	m_side;


#define	MAX_TEMP_ENTITIES	64			// lightning bolts, etc
#define	MAX_STATIC_ENTITIES	128			// torches, etc

EXTERN 	client_state_t	cl;

// FIXME, allocate dynamically
EXTERN 	efrag_t			cl_efrags[MAX_EFRAGS];
EXTERN 	entity_t		cl_entities[MAX_EDICTS];
EXTERN 	entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
EXTERN 	lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
EXTERN 	dlight_t		cl_dlights[MAX_DLIGHTS];
EXTERN 	entity_t		cl_temp_entities[MAX_TEMP_ENTITIES];
EXTERN 	beam_t			cl_beams[MAX_BEAMS];

//=============================================================================

//
// cl_main
//


	dlight_t *CL_AllocDlight(int key);
	void	CL_DecayLights(void);

	EXTERN_CPP void CL_Init(void);

	EXTERN_CPP void CL_EstablishConnection(char *host);
	EXTERN_CPP void CL_Signon1(void);
	EXTERN_CPP void CL_Signon2(void);
	EXTERN_CPP void CL_Signon3(void);
	EXTERN_CPP void CL_Signon4(void);

	EXTERN_CPP void CL_Disconnect(void);
	EXTERN_CPP void CL_Disconnect_f(void);
	EXTERN_CPP void CL_NextDemo(void);

#define			MAX_VISEDICTS	256
	EXTERN	int				cl_numvisedicts;
	EXTERN	entity_t		*cl_visedicts[MAX_VISEDICTS];

	//
	// cl_input
	//
	typedef struct
	{
		int		down[2];		// key nums holding it down
		int		state;			// low bit is down state
	} kbutton_t;

	EXTERN	kbutton_t	in_mlook, in_klook;
	EXTERN 	kbutton_t 	in_strafe;
	EXTERN 	kbutton_t 	in_speed;

	EXTERN_CPP void CL_InitInput(void);
	EXTERN_CPP void CL_SendCmd(void);
	EXTERN_CPP void CL_SendMove(usercmd_t *cmd);

	EXTERN_CPP void CL_ParseTEnt(void);
	EXTERN_CPP void CL_UpdateTEnts(void);

	EXTERN_CPP void CL_ClearState(void);


	EXTERN_CPP int  CL_ReadFromServer(void);
	EXTERN_CPP void CL_WriteToServer(usercmd_t *cmd);
	EXTERN_CPP void CL_BaseMove(usercmd_t *cmd);


	EXTERN_CPP float CL_KeyState(kbutton_t *key);
	EXTERN_CPP char *Key_KeynumToString(int keynum);

	//
	// cl_demo.c
	//
	EXTERN_CPP void CL_StopPlayback(void);
	EXTERN_CPP int CL_GetMessage(void);

	EXTERN_CPP void CL_Stop_f(void);
	EXTERN_CPP void CL_Record_f(void);
	EXTERN_CPP void CL_PlayDemo_f(void);
	EXTERN_CPP void CL_TimeDemo_f(void);

	//
	// cl_parse.c
	//
	EXTERN_CPP void CL_ParseServerMessage(void);
	EXTERN_CPP void CL_NewTranslation(int slot);

	//
	// view
	//
	EXTERN_CPP void V_StartPitchDrift(void);
	EXTERN_CPP void V_StopPitchDrift(void);

	EXTERN_CPP void V_RenderView(void);
	EXTERN_CPP void V_UpdatePalette(void);
	EXTERN_CPP void V_Register(void);
	EXTERN_CPP void V_ParseDamage(void);
	EXTERN_CPP void V_SetContentsColor(int contents);


	//
	// cl_tent
	//
	EXTERN_CPP void CL_InitTEnts(void);
	EXTERN_CPP void CL_SignonReply(void);
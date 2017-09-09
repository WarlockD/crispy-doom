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

//
// console
//
#if 0
EXTERN  int con_totallines;
EXTERN  int con_backscroll;
EXTERN 	qboolean con_forcedup;	// because no entities to refresh
EXTERN  qboolean con_initialized;
EXTERN  byte *con_chars;
EXTERN 	int	con_notifylines;		// scan lines to clear for notify lines

EXTERN_CPP void Con_DrawCharacter (int cx, int line, int num);

EXTERN_CPP void Con_CheckResize (void);
EXTERN_CPP void Con_Init (void);
EXTERN_CPP void Con_DrawConsole (int lines, qboolean drawinput);
EXTERN_CPP void Con_Print (char *txt);
EXTERN_CPP void Con_Printf (char *fmt, ...);
EXTERN_CPP void Con_DPrintf (char *fmt, ...);
EXTERN_CPP void Con_SafePrintf (char *fmt, ...);
EXTERN_CPP void Con_Clear_f (void);
EXTERN_CPP void Con_DrawNotify (void);
EXTERN_CPP void Con_ClearNotify (void);
EXTERN_CPP void Con_ToggleConsole_f (void);

EXTERN_CPP void Con_NotifyBox (char *text);	// during startup for sound / cd warnings
#endif

#define		CON_TEXTSIZE	16384
typedef struct
{
	char	text[CON_TEXTSIZE];
	int		current;		// line where next message will be printed
	int		x;				// offset in current line for next print
	int		display;		// bottom of console displays this line
} console_t;

EXTERN	console_t	con_main;
EXTERN	console_t	con_chat;
EXTERN	console_t	*con;			// point to either con_main or con_chat

EXTERN	int			con_ormask;

EXTERN int con_totallines;
EXTERN qboolean con_initialized;
EXTERN byte *con_chars;
EXTERN	int	con_notifylines;		// scan lines to clear for notify lines
EXTERN qboolean 	con_forcedup;
EXTERN_CPP void Con_DrawCharacter (int cx, int line, int num);

EXTERN_CPP void Con_CheckResize (void);
EXTERN_CPP void Con_Init (void);
//EXTERN_CPP void Con_DrawConsole (int lines);
EXTERN_CPP void Con_DrawConsole (int lines, qboolean drawinput);
EXTERN_CPP void Con_Print (char *txt);
EXTERN_CPP void Con_Printf (char *fmt, ...);
EXTERN_CPP void Con_DPrintf (char *fmt, ...);
EXTERN_CPP void Con_SafePrintf (char *fmt, ...);
EXTERN_CPP void Con_Clear_f (void);
EXTERN_CPP void Con_DrawNotify (void);
EXTERN_CPP void Con_ClearNotify (void);
EXTERN_CPP void Con_ToggleConsole_f (void);

EXTERN_CPP void Con_NotifyBox (char *text);	// during startup for sound / cd warnings

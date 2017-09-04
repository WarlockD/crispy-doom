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
// screen.h

EXTERN_CPP void SCR_Init (void);

EXTERN_CPP void SCR_UpdateScreen (void);


EXTERN_CPP void SCR_SizeUp (void);
EXTERN_CPP void SCR_SizeDown (void);
EXTERN_CPP void SCR_BringDownConsole (void);
EXTERN_CPP void SCR_CenterPrint (char *str);

EXTERN_CPP void SCR_BeginLoadingPlaque (void);
EXTERN_CPP void SCR_EndLoadingPlaque (void);

EXTERN_CPP int SCR_ModalMessage (char *text);

EXTERN 	float		scr_con_current;
EXTERN 	float		scr_conlines;		// lines of console to display

EXTERN 	int			scr_fullupdate;	// set to 0 to force full redraw
EXTERN 	int			sb_lines;

EXTERN 	int			clearnotify;	// set to 0 whenever notify text is drawn
EXTERN 	qboolean	scr_disabled_for_loading;
EXTERN 	qboolean	scr_skipupdate;

EXTERN 	cvar_t		scr_viewsize;

EXTERN  cvar_t scr_viewsize;

// only the refresh window will be updated unless these variables are flagged 
EXTERN 	int			scr_copytop;
EXTERN 	int			scr_copyeverything;

EXTERN  qboolean		block_drawing;

EXTERN_CPP void SCR_UpdateWholeScreen (void);

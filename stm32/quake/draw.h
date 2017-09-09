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

// draw.h -- these are the only functions outside the refresh allowed
// to touch the vid buffer

EXTERN 	qpic_t		*draw_disc;	// also used on sbar

EXTERN_CPP void Draw_Init (void);
EXTERN_CPP void Draw_Character (int x, int y, int num);
EXTERN_CPP void Draw_DebugChar (char num);
EXTERN_CPP void Draw_Pic (int x, int y, qpic_t *pic);
EXTERN_CPP void Draw_TransPic (int x, int y, qpic_t *pic);
EXTERN_CPP void Draw_TransPicTranslate (int x, int y, qpic_t *pic, byte *translation);
EXTERN_CPP void Draw_ConsoleBackground (int lines);
EXTERN_CPP void Draw_BeginDisc (void);
EXTERN_CPP void Draw_EndDisc (void);
EXTERN_CPP void Draw_TileClear (int x, int y, int w, int h);
EXTERN_CPP void Draw_Fill (int x, int y, int w, int h, int c);
EXTERN_CPP void Draw_FadeScreen (void);
EXTERN_CPP void Draw_String (int x, int y, char *str);
EXTERN_CPP qpic_t *Draw_PicFromWad (char *name);
EXTERN_CPP qpic_t *Draw_CachePic (char *path);

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
// vid_dos.h: header file for DOS-specific video stuff

typedef struct vmode_s {
	struct vmode_s	*pnext;
	char		*name;
	char		*header;
	unsigned	width;
	unsigned	height;
	float		aspect;
	unsigned	rowbytes;
	int			planar;
	int			numpages;
	void		*pextradata;
	int			(*setmode)(viddef_t *vid, struct vmode_s *pcurrentmode);
	void		(*swapbuffers)(viddef_t *vid, struct vmode_s *pcurrentmode,
							   vrect_t *rects);
	void		(*setpalette)(viddef_t *vid, struct vmode_s *pcurrentmode,
							  unsigned char *palette);
	void		(*begindirectrect)(viddef_t *vid, struct vmode_s *pcurrentmode,
								   int x, int y, byte *pbitmap, int width,
								   int height);
	void		(*enddirectrect)(viddef_t *vid, struct vmode_s *pcurrentmode,
								 int x, int y, int width, int height);
} vmode_t;

// vid_wait settings
#define VID_WAIT_NONE			0
#define VID_WAIT_VSYNC			1
#define VID_WAIT_DISPLAY_ENABLE	2

EXTERN  int		numvidmodes;
EXTERN  vmode_t	*pvidmodes;

EXTERN  int		VGA_width, VGA_height, VGA_rowbytes, VGA_bufferrowbytes;
EXTERN  byte		*VGA_pagebase;
EXTERN  vmode_t	*VGA_pcurmode;

EXTERN  cvar_t	vid_wait;
EXTERN  cvar_t	vid_nopageflip;
EXTERN  cvar_t	_vid_wait_override;

EXTERN  unsigned char colormap256[32][256];

EXTERN  void	*vid_surfcache;
EXTERN  int	vid_surfcachesize;

EXTERN_CPP void VGA_Init (void);
EXTERN_CPP void VID_InitVESA (void);
EXTERN_CPP void VID_InitExtra (void);
EXTERN_CPP void VGA_WaitVsync (void);
EXTERN_CPP void VGA_ClearVideoMem (int planar);
EXTERN_CPP void VGA_SetPalette(viddef_t *vid, vmode_t *pcurrentmode, unsigned char *pal);
EXTERN_CPP void VGA_SwapBuffersCopy (viddef_t *vid, vmode_t *pcurrentmode,
	vrect_t *rects);
EXTERN_CPP qboolean VGA_FreeAndAllocVidbuffer (viddef_t *vid, int allocnewbuffer);
EXTERN_CPP qboolean VGA_CheckAdequateMem (int width, int height, int rowbytes,
	int allocnewbuffer);
EXTERN_CPP void VGA_BeginDirectRect (viddef_t *vid, struct vmode_s *pcurrentmode, int x,
	int y, byte *pbitmap, int width, int height);
EXTERN_CPP void VGA_EndDirectRect (viddef_t *vid, struct vmode_s *pcurrentmode, int x,
	int y, int width, int height);
EXTERN_CPP void VGA_UpdateLinearScreen (void *srcptr, void *destptr, int width,
	int height, int srcrowbytes, int destrowbytes);


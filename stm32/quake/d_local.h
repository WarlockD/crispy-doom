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
// d_local.h:  private rasterization driver defs

#include "r_shared.h"

//
// TODO: fine-tune this; it's based on providing some overage even if there
// is a 2k-wide scan, with subdivision every 8, for 256 spans of 12 bytes each
//
#define SCANBUFFERPAD		0x1000

#define R_SKY_SMASK	0x007F0000
#define R_SKY_TMASK	0x007F0000

#define DS_SPAN_LIST_END	-128

#define SURFCACHE_SIZE_AT_320X200	600*1024

typedef struct surfcache_s
{
	struct surfcache_s	*next;
	struct surfcache_s 	**owner;		// NULL is an empty chunk of memory
	int					lightadj[MAXLIGHTMAPS]; // checked for strobe flush
	int					dlight;
	int					size;		// including header
	unsigned			width;
	unsigned			height;		// DEBUG only needed for debug
	float				mipscale;
	struct texture_s	*texture;	// checked for animating textures
	byte				data[4];	// width*height elements
} surfcache_t;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct sspan_s
{
	int				u, v, count;
} sspan_t;

EXTERN  cvar_t	d_subdiv16;

EXTERN  float	scale_for_mip;

EXTERN  qboolean		d_roverwrapped;
EXTERN  surfcache_t	*sc_rover;
EXTERN  surfcache_t	*d_initial_rover;

EXTERN  float	d_sdivzstepu, d_tdivzstepu, d_zistepu;
EXTERN  float	d_sdivzstepv, d_tdivzstepv, d_zistepv;
EXTERN  float	d_sdivzorigin, d_tdivzorigin, d_ziorigin;

fixed16_t	sadjust, tadjust;
fixed16_t	bbextents, bbextentt;


EXTERN_CPP void D_DrawSpans8 (espan_t *pspans);
EXTERN_CPP void D_DrawSpans16 (espan_t *pspans);
EXTERN_CPP void D_DrawZSpans (espan_t *pspans);
EXTERN_CPP void Turbulent8 (espan_t *pspan);
EXTERN_CPP void D_SpriteDrawSpans (sspan_t *pspan);

EXTERN_CPP void D_DrawSkyScans8 (espan_t *pspan);
EXTERN_CPP void D_DrawSkyScans16 (espan_t *pspan);

EXTERN_CPP void R_ShowSubDiv (void);
void (*prealspandrawer)(void);
EXTERN_CPP surfcache_t	*D_CacheSurface (msurface_t *surface, int miplevel);

EXTERN_CPP int D_MipLevelForScale (float scale);

#if id386
EXTERN  void D_PolysetAff8Start (void);
EXTERN  void D_PolysetAff8End (void);
#endif

EXTERN  short *d_pzbuffer;
EXTERN  unsigned int d_zrowbytes, d_zwidth;

EXTERN  int	*d_pscantable;
EXTERN  int	d_scantable[MAXHEIGHT];

EXTERN  int	d_vrectx, d_vrecty, d_vrectright_particle, d_vrectbottom_particle;

EXTERN  int	d_y_aspect_shift, d_pix_min, d_pix_max, d_pix_shift;

EXTERN  pixel_t	*d_viewbuffer;

EXTERN  short	*zspantable[MAXHEIGHT];

EXTERN  int		d_minmip;
EXTERN  float	d_scalemip[3];

EXTERN  void (*d_drawspans) (espan_t *pspan);


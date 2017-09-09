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
// d_iface.h: interface header file for rasterization driver modules

#define WARP_WIDTH		320
#define WARP_HEIGHT		200

#define MAX_LBM_HEIGHT	480

typedef struct
{
	float	u, v;
	float	s, t;
	float	zi;
} emitpoint_t;

typedef enum {
	pt_static, pt_grav, pt_slowgrav, pt_fire, pt_explode, pt_explode2, pt_blob, pt_blob2
} ptype_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct particle_s
{
// driver-usable fields
	vec3_t		org;
	float		color;
// drivers never touch the following fields
	struct particle_s	*next;
	vec3_t		vel;
	float		ramp;
	float		die;
	ptype_t		type;
} particle_t;

#define PARTICLE_Z_CLIP	8.0

typedef struct polyvert_s {
	float	u, v, zi, s, t;
} polyvert_t;

typedef struct polydesc_s {
	int			numverts;
	float		nearzi;
	msurface_t	*pcurrentface;
	polyvert_t	*pverts;
} polydesc_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct finalvert_s {
	int		v[6];		// u, v, s, t, l, 1/z
	int		flags;
	float	reserved;
} finalvert_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct
{
	void				*pskin;
	maliasskindesc_t	*pskindesc;
	int					skinwidth;
	int					skinheight;
	mtriangle_t			*ptriangles;
	finalvert_t			*pfinalverts;
	int					numtriangles;
	int					drawtype;
	int					seamfixupX16;
} affinetridesc_t;

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
typedef struct {
	float	u, v, zi, color;
} screenpart_t;

typedef struct
{
	int			nump;
	emitpoint_t	*pverts;	// there's room for an extra element at [nump], 
							//  if the driver wants to duplicate element [0] at
							//  element [nump] to avoid dealing with wrapping
	mspriteframe_t	*pspriteframe;
	vec3_t			vup, vright, vpn;	// in worldspace
	float			nearzi;
} spritedesc_t;

typedef struct
{
	int		u, v;
	float	zi;
	int		color;
} zpointdesc_t;

EXTERN  cvar_t	r_drawflat;
EXTERN  int		d_spanpixcount;
EXTERN  int		r_framecount;		// sequence # of current frame since Quake
									//  started
EXTERN  qboolean	r_drawpolys;		// 1 if driver wants clipped polygons
									//  rather than a span list
EXTERN  qboolean	r_drawculledpolys;	// 1 if driver wants clipped polygons that
									//  have been culled by the edge list
EXTERN  qboolean	r_worldpolysbacktofront;	// 1 if driver wants polygons
											//  delivered back to front rather
											//  than front to back
EXTERN  qboolean	r_recursiveaffinetriangles;	// true if a driver wants to use
											//  recursive triangular subdivison
											//  and vertex drawing via
											//  D_PolysetDrawFinalVerts() past
											//  a certain distance (normally 
											//  only used by the software
											//  driver)
EXTERN  float	r_aliasuvscale;		// scale-up factor for screen u and v
									//  on Alias vertices passed to driver
EXTERN  int		r_pixbytes;
EXTERN  qboolean	r_dowarp;

EXTERN  affinetridesc_t	r_affinetridesc;
EXTERN  spritedesc_t		r_spritedesc;
EXTERN  zpointdesc_t		r_zpointdesc;
EXTERN  polydesc_t		r_polydesc;

EXTERN  int		d_con_indirect;	// if 0, Quake will draw console directly
								//  to vid.buffer; if 1, Quake will
								//  draw console via D_DrawRect. Must be
								//  defined by driver

EXTERN  vec3_t	r_pright, r_pup, r_ppn;


EXTERN_CPP void D_Aff8Patch (void *pcolormap);
EXTERN_CPP void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height);
EXTERN_CPP void D_DisableBackBufferAccess (void);
EXTERN_CPP void D_EndDirectRect (int x, int y, int width, int height);
EXTERN_CPP void D_PolysetDraw (void);
EXTERN_CPP void D_PolysetDrawFinalVerts (finalvert_t *fv, int numverts);
EXTERN_CPP void D_DrawParticle (particle_t *pparticle);
EXTERN_CPP void D_DrawPoly (void);
EXTERN_CPP void D_DrawSprite (void);
EXTERN_CPP void D_DrawSurfaces (void);
EXTERN_CPP void D_DrawZPoint (void);
EXTERN_CPP void D_EnableBackBufferAccess (void);
EXTERN_CPP void D_EndParticles (void);
EXTERN_CPP void D_Init (void);
EXTERN_CPP void D_ViewChanged (void);
EXTERN_CPP void D_SetupFrame (void);
EXTERN_CPP void D_StartParticles (void);
EXTERN_CPP void D_TurnZOn (void);
EXTERN_CPP void D_WarpScreen (void);

EXTERN_CPP void D_FillRect (vrect_t *vrect, int color);
EXTERN_CPP void D_DrawRect (void);
EXTERN_CPP void D_UpdateRects (vrect_t *prect);

// currently for internal use only, and should be a do-nothing function in
// hardware drivers
// FIXME: this should go away
EXTERN_CPP void D_PolysetUpdateTables (void);

// these are currently for internal use only, and should not be used by drivers
EXTERN  int				r_skydirect;
EXTERN  byte				*r_skysource;

// transparency types for D_DrawRect ()
#define DR_SOLID		0
#define DR_TRANSPARENT	1

// !!! must be kept the same as in quakeasm.h !!!
#define TRANSPARENT_COLOR	0xFF

EXTERN  void *acolormap;	// FIXME: should go away

//=======================================================================//

// callbacks to Quake

typedef struct
{
	pixel_t		*surfdat;	// destination for generated surface
	int			rowbytes;	// destination logical width in bytes
	msurface_t	*surf;		// description for surface to generate
	fixed8_t	lightadj[MAXLIGHTMAPS];
							// adjust for lightmap levels for dynamic lighting
	texture_t	*texture;	// corrected for animating textures
	int			surfmip;	// mipmapped ratio of surface texels / world pixels
	int			surfwidth;	// in mipmapped texels
	int			surfheight;	// in mipmapped texels
} drawsurf_t;

EXTERN  drawsurf_t	r_drawsurf;

EXTERN_CPP void R_DrawSurface (void);
EXTERN_CPP void R_GenTile (msurface_t *psurf, void *pdest);


// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define TURB_TEX_SIZE	64		// base turbulent texture size

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define	CYCLE			128		// turbulent cycle size

#define TILE_SIZE		128		// size of textures generated by R_GenTiledSurf

#define SKYSHIFT		7
#define	SKYSIZE			(1 << SKYSHIFT)
#define SKYMASK			(SKYSIZE - 1)

EXTERN  float	skyspeed, skyspeed2;
EXTERN  float	skytime;

EXTERN  int		c_surf;
EXTERN  vrect_t	scr_vrect;

EXTERN  byte		*r_warpbuffer;


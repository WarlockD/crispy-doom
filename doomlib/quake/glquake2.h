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
// disable data conversion warnings

#pragma warning(disable : 4244)     // MIPS
#pragma warning(disable : 4136)     // X86
#pragma warning(disable : 4051)     // ALPHA
  
#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>

EXTERN_CPP void GL_BeginRendering (int *x, int *y, int *width, int *height);
EXTERN_CPP void GL_EndRendering (void);


// Function prototypes for the Texture Object Extension routines
typedef GLboolean (APIENTRY *ARETEXRESFUNCPTR)(GLsizei, const GLuint *,
                    const GLboolean *);
typedef void (APIENTRY *BINDTEXFUNCPTR)(GLenum, GLuint);
typedef void (APIENTRY *DELTEXFUNCPTR)(GLsizei, const GLuint *);
typedef void (APIENTRY *GENTEXFUNCPTR)(GLsizei, GLuint *);
typedef GLboolean (APIENTRY *ISTEXFUNCPTR)(GLuint);
typedef void (APIENTRY *PRIORTEXFUNCPTR)(GLsizei, const GLuint *,
                    const GLclampf *);
typedef void (APIENTRY *TEXSUBIMAGEPTR)(int, int, int, int, int, int, int, int, void *);

EXTERN 	BINDTEXFUNCPTR bindTexFunc;
EXTERN 	DELTEXFUNCPTR delTexFunc;
EXTERN 	TEXSUBIMAGEPTR TexSubImage2DFunc;

EXTERN 	int texture_extension_number;
EXTERN 	int		texture_mode;

EXTERN 	float	gldepthmin, gldepthmax;

EXTERN_CPP void GL_Upload32 (unsigned *data, int width, int height,  qboolean mipmap, qboolean alpha, qboolean modulate);
EXTERN_CPP void GL_Upload8 (byte *data, int width, int height,  qboolean mipmap, qboolean alpha, qboolean modulate);
EXTERN_CPP int GL_LoadTexture (char *identifier, int width, int height, byte *data, int mipmap, int alpha, int modulate);
EXTERN_CPP int GL_FindTexture (char *identifier);

typedef struct
{
	float	x, y, z;
	float	s, t;
	float	r, g, b;
} glvert_t;

EXTERN  glvert_t glv;

EXTERN 	int glx, gly, glwidth, glheight;

EXTERN 	PROC glArrayElementEXT;
EXTERN 	PROC glColorPointerEXT;
EXTERN 	PROC glTexturePointerEXT;
EXTERN 	PROC glVertexPointerEXT;


// r_local.h -- private refresh defs

#define MAXALIASVERTS		2000	// TODO: tune this

#define ALIAS_BASE_SIZE_RATIO		(1.0 / 11.0)
					// normalizing factor so player model works out to about
					//  1 pixel per triangle
#define	MAX_LBM_HEIGHT		480

#define TILE_SIZE		128		// size of textures generated by R_GenTiledSurf

#define SKYSHIFT		7
#define	SKYSIZE			(1 << SKYSHIFT)
#define SKYMASK			(SKYSIZE - 1)

#define BACKFACE_EPSILON	0.01


EXTERN_CPP void R_TimeRefresh_f (void);
EXTERN_CPP void R_ReadPointFile_f (void);
EXTERN_CPP texture_t *R_TextureAnimation (texture_t *base);

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


//====================================================


EXTERN 	entity_t	r_worldentity;
EXTERN 	qboolean	r_cache_thrash;		// compatability
EXTERN 	vec3_t		modelorg, r_entorigin;
EXTERN 	entity_t	*currententity;
EXTERN 	int			r_visframecount;	// ??? what difs?
EXTERN 	int			r_framecount;
EXTERN 	mplane_t	frustum[4];
EXTERN 	int		c_brush_polys, c_alias_polys;


//
// view origin
//
EXTERN 	vec3_t	vup;
EXTERN 	vec3_t	vpn;
EXTERN 	vec3_t	vright;
EXTERN 	vec3_t	r_origin;

//
// screen size info
//
EXTERN 	refdef_t	r_refdef;
EXTERN 	mleaf_t		*r_viewleaf, *r_oldviewleaf;
EXTERN 	texture_t	*r_notexture_mip;
EXTERN 	int		d_lightstylevalue[256];	// 8.8 fraction of base light value

EXTERN 	qboolean	envmap;
EXTERN 	int	currenttexture;
EXTERN 	int	particletexture;
EXTERN 	int	playertextures;

EXTERN 	int	skytexturenum;		// index in cl.loadmodel, not gl texture object

EXTERN 	cvar_t	r_drawentities;
EXTERN 	cvar_t	r_drawworld;
EXTERN 	cvar_t	r_drawviewmodel;
EXTERN 	cvar_t	r_speeds;
EXTERN 	cvar_t	r_waterwarp;
EXTERN 	cvar_t	r_fullbright;
EXTERN 	cvar_t	r_lightmap;
EXTERN 	cvar_t	r_shadows;
EXTERN 	cvar_t	r_dynamic;

EXTERN 	cvar_t	gl_clear;
EXTERN 	cvar_t	gl_cull;
EXTERN 	cvar_t	gl_poly;
EXTERN 	cvar_t	gl_texsort;
EXTERN 	cvar_t	gl_smoothmodels;
EXTERN 	cvar_t	gl_affinemodels;
EXTERN 	cvar_t	gl_fogblend;
EXTERN 	cvar_t	gl_polyblend;
EXTERN 	cvar_t	gl_keeptjunctions;
EXTERN 	cvar_t	gl_reporttjunctions;

EXTERN 	int		gl_lightmap_format;
EXTERN 	int		gl_solid_format;
EXTERN 	int		gl_alpha_format;

EXTERN_CPP void R_TranslatePlayerSkin (int playernum);
EXTERN_CPP void GL_Bind (int texnum);
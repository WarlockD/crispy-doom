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
// r_local.h -- private refresh defs

#ifndef GLQUAKE
#include "r_shared.h"

#define ALIAS_BASE_SIZE_RATIO		(1.0 / 11.0)
					// normalizing factor so player model works out to about
					//  1 pixel per triangle

#define BMODEL_FULLY_CLIPPED	0x10 // value returned by R_BmodelCheckBBox ()
									 //  if bbox is trivially rejected

//===========================================================================
// viewmodel lighting

typedef struct {
	int			ambientlight;
	int			shadelight;
	float		*plightvec;
} alight_t;

//===========================================================================
// clipped bmodel edges

typedef struct bedge_s
{
	mvertex_t		*v[2];
	struct bedge_s	*pnext;
} bedge_t;

typedef struct {
	float	fv[3];		// viewspace x, y
} auxvert_t;

//===========================================================================

EXTERN  cvar_t	r_draworder;
EXTERN  cvar_t	r_speeds;
EXTERN  cvar_t	r_timegraph;
EXTERN  cvar_t	r_graphheight;
EXTERN  cvar_t	r_clearcolor;
EXTERN  cvar_t	r_waterwarp;
EXTERN  cvar_t	r_fullbright;
EXTERN  cvar_t	r_drawentities;
EXTERN  cvar_t	r_aliasstats;
EXTERN  cvar_t	r_dspeeds;
EXTERN  cvar_t	r_drawflat;
EXTERN  cvar_t	r_ambient;
EXTERN  cvar_t	r_reportsurfout;
EXTERN  cvar_t	r_maxsurfs;
EXTERN  cvar_t	r_numsurfs;
EXTERN  cvar_t	r_reportedgeout;
EXTERN  cvar_t	r_maxedges;
EXTERN  cvar_t	r_numedges;

#define XCENTERING	(1.0 / 2.0)
#define YCENTERING	(1.0 / 2.0)

#define CLIP_EPSILON		0.001

#define BACKFACE_EPSILON	0.01

//===========================================================================

#define	DIST_NOT_SET	98765

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct clipplane_s
{
	vec3_t		normal;
	float		dist;
	struct		clipplane_s	*next;
	byte		leftedge;
	byte		rightedge;
	byte		reserved[2];
} clipplane_t;

EXTERN 	clipplane_t	view_clipplanes[4];

//=============================================================================

EXTERN_CPP void R_RenderWorld (void);

//=============================================================================

EXTERN 	mplane_t	screenedge[4];

EXTERN 	vec3_t	r_origin;

EXTERN 	vec3_t	r_entorigin;

EXTERN 	float	screenAspect;
EXTERN 	float	verticalFieldOfView;
EXTERN 	float	xOrigin, yOrigin;

EXTERN 	int		r_visframecount;

//=============================================================================

EXTERN  int	vstartscan;


EXTERN_CPP void R_ClearPolyList (void);
EXTERN_CPP void R_DrawPolyList (void);

//
// current entity info
//
EXTERN 	qboolean		insubmodel;
EXTERN 	vec3_t			r_worldmodelorg;


EXTERN_CPP void R_DrawSprite (void);
EXTERN_CPP void R_RenderFace (msurface_t *fa, int clipflags);
EXTERN_CPP void R_RenderPoly (msurface_t *fa, int clipflags);
EXTERN_CPP void R_RenderBmodelFace (bedge_t *pedges, msurface_t *psurf);
EXTERN_CPP void R_TransformPlane (mplane_t *p, float *normal, float *dist);
EXTERN_CPP void R_TransformFrustum (void);
EXTERN_CPP void R_SetSkyFrame (void);
EXTERN_CPP void R_DrawSurfaceBlock16 (void);
EXTERN_CPP void R_DrawSurfaceBlock8 (void);
EXTERN_CPP texture_t *R_TextureAnimation (texture_t *base);

#if	id386

EXTERN_CPP void R_DrawSurfaceBlock8_mip0 (void);
EXTERN_CPP void R_DrawSurfaceBlock8_mip1 (void);
EXTERN_CPP void R_DrawSurfaceBlock8_mip2 (void);
EXTERN_CPP void R_DrawSurfaceBlock8_mip3 (void);

#endif

EXTERN_CPP void R_GenSkyTile (void *pdest);
EXTERN_CPP void R_GenSkyTile16 (void *pdest);
EXTERN_CPP void R_Surf8Patch (void);
EXTERN_CPP void R_Surf16Patch (void);
EXTERN_CPP void R_DrawSubmodelPolygons (model_t *pmodel, int clipflags);
EXTERN_CPP void R_DrawSolidClippedSubmodelPolygons (model_t *pmodel);

EXTERN_CPP void R_AddPolygonEdges (emitpoint_t *pverts, int numverts, int miplevel);
EXTERN_CPP surf_t *R_GetSurf (void);
EXTERN_CPP void R_AliasDrawModel (alight_t *plighting);
EXTERN_CPP void R_BeginEdgeFrame (void);
EXTERN_CPP void R_ScanEdges (void);
EXTERN_CPP void D_DrawSurfaces (void);
EXTERN_CPP void R_InsertNewEdges (edge_t *edgestoadd, edge_t *edgelist);
EXTERN_CPP void R_StepActiveU (edge_t *pedge);
EXTERN_CPP void R_RemoveEdges (edge_t *pedge);

EXTERN  void R_Surf8Start (void);
EXTERN  void R_Surf8End (void);
EXTERN  void R_Surf16Start (void);
EXTERN  void R_Surf16End (void);
EXTERN  void R_EdgeCodeStart (void);
EXTERN  void R_EdgeCodeEnd (void);

EXTERN  void R_RotateBmodel (void);

EXTERN  int	c_faceclip;
EXTERN  int	r_polycount;
EXTERN  int	r_wholepolycount;

EXTERN 	model_t		*cl_worldmodel;

EXTERN  int		*pfrustum_indexes[4];

// !!! if this is changed, it must be changed in asm_draw.h too !!!
#define	NEAR_CLIP	0.01

EXTERN  int			ubasestep, errorterm, erroradjustup, erroradjustdown;
EXTERN  int			vstartscan;

EXTERN  fixed16_t	sadjust, tadjust;
EXTERN  fixed16_t	bbextents, bbextentt;

#define MAXBVERTINDEXES	1000	// new clipped vertices when clipping bmodels
								//  to the world BSP
EXTERN  mvertex_t	*r_ptverts, *r_ptvertsmax;

EXTERN  vec3_t			sbaseaxis[3], tbaseaxis[3];
EXTERN  float			entity_rotation[3][3];

EXTERN  int		reinit_surfcache;

EXTERN  int		r_currentkey;
EXTERN  int		r_currentbkey;

typedef struct btofpoly_s {
	int			clipflags;
	msurface_t	*psurf;
} btofpoly_t;

#define MAX_BTOFPOLYS	5000	// FIXME: tune this

EXTERN  int			numbtofpolys;
EXTERN  btofpoly_t	*pbtofpolys;

EXTERN_CPP void	R_InitTurb (void);
EXTERN_CPP void	R_ZDrawSubmodelPolys (model_t *clmodel);

//=========================================================
// Alias models
//=========================================================

#define MAXALIASVERTS		2000	// TODO: tune this
#define ALIAS_Z_CLIP_PLANE	5

EXTERN  int				numverts;
EXTERN  int				a_skinwidth;
EXTERN  mtriangle_t		*ptriangles;
EXTERN  int				numtriangles;
EXTERN  aliashdr_t		*paliashdr;
EXTERN  mdl_t			*pmdl;
EXTERN  float			leftclip, topclip, rightclip, bottomclip;
EXTERN  int				r_acliptype;
EXTERN  finalvert_t		*pfinalverts;
EXTERN  auxvert_t		*pauxverts;

EXTERN_CPP qboolean R_AliasCheckBBox (void);

//=========================================================
// turbulence stuff

#define	AMP		8*0x10000
#define	AMP2	3
#define	SPEED	20

//=========================================================
// particle stuff

EXTERN_CPP void R_DrawParticles (void);
EXTERN_CPP void R_InitParticles (void);
EXTERN_CPP void R_ClearParticles (void);
EXTERN_CPP void R_ReadPointFile_f (void);
EXTERN_CPP void R_SurfacePatch (void);

EXTERN  int		r_amodels_drawn;
EXTERN  edge_t	*auxedges;
EXTERN  int		r_numallocatededges;
EXTERN  edge_t	*r_edges, *edge_p, *edge_max;

EXTERN 	edge_t	*newedges[MAXHEIGHT];
EXTERN 	edge_t	*removeedges[MAXHEIGHT];

EXTERN 	int	screenwidth;

// FIXME: make stack vars when debugging done
EXTERN 	edge_t	edge_head;
EXTERN 	edge_t	edge_tail;
EXTERN 	edge_t	edge_aftertail;
EXTERN  int		r_bmodelactive;
EXTERN  vrect_t	*pconupdate;

EXTERN  float		aliasxscale, aliasyscale, aliasxcenter, aliasycenter;
EXTERN  float		r_aliastransition, r_resfudge;

EXTERN  int		r_outofsurfaces;
EXTERN  int		r_outofedges;

EXTERN  mvertex_t	*r_pcurrentvertbase;
EXTERN  int			r_maxvalidedgeoffset;

EXTERN_CPP void R_AliasClipTriangle (mtriangle_t *ptri);

EXTERN  float	r_time1;
EXTERN  float	dp_time1, dp_time2, db_time1, db_time2, rw_time1, rw_time2;
EXTERN  float	se_time1, se_time2, de_time1, de_time2, dv_time1, dv_time2;
EXTERN  int		r_frustum_indexes[4*6];
EXTERN  int		r_maxsurfsseen, r_maxedgesseen, r_cnumsurfs;
EXTERN  qboolean	r_surfsonstack;
EXTERN  cshift_t	cshift_water;
EXTERN  qboolean	r_dowarpold, r_viewchanged;

EXTERN  mleaf_t	*r_viewleaf, *r_oldviewleaf;

EXTERN  vec3_t	r_emins, r_emaxs;
EXTERN  mnode_t	*r_pefragtopnode;
EXTERN  int		r_clipflags;
EXTERN  int		r_dlightframecount;
EXTERN  qboolean	r_fov_greater_than_90;

EXTERN_CPP void R_StoreEfrags (efrag_t **ppefrag);
EXTERN_CPP void R_TimeRefresh_f (void);
EXTERN_CPP void R_TimeGraph (void);
EXTERN_CPP void R_PrintAliasStats (void);
EXTERN_CPP void R_PrintTimes (void);
EXTERN_CPP void R_PrintDSpeeds (void);
EXTERN_CPP void R_AnimateLight (void);
EXTERN_CPP int R_LightPoint (vec3_t p);
EXTERN_CPP void R_SetupFrame (void);
EXTERN_CPP void R_cshift_f (void);
EXTERN_CPP void R_EmitEdge (mvertex_t *pv0, mvertex_t *pv1);
EXTERN_CPP void R_ClipEdge (mvertex_t *pv0, mvertex_t *pv1, clipplane_t *clip);
EXTERN_CPP void R_SplitEntityOnNode2 (mnode_t *node);
EXTERN_CPP void R_MarkLights (dlight_t *light, int bit, mnode_t *node);

#endif
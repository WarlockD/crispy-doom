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


// upper design bounds

#define	MAX_MAP_HULLS		4

#define	MAX_MAP_MODELS		256
#define	MAX_MAP_BRUSHES		4096
#define	MAX_MAP_ENTITIES	1024
#define	MAX_MAP_ENTSTRING	65536

#define	MAX_MAP_PLANES		32767
#define	MAX_MAP_NODES		32767		// because negative shorts are contents
#define	MAX_MAP_CLIPNODES	32767		//
#define	MAX_MAP_LEAFS		8192
#define	MAX_MAP_VERTS		65535
#define	MAX_MAP_FACES		65535
#define	MAX_MAP_MARKSURFACES 65535
#define	MAX_MAP_TEXINFO		4096
#define	MAX_MAP_EDGES		256000
#define	MAX_MAP_SURFEDGES	512000
#define	MAX_MAP_TEXTURES	512
#define	MAX_MAP_MIPTEX		0x200000
#define	MAX_MAP_LIGHTING	0x100000
#define	MAX_MAP_VISIBILITY	0x100000

#define	MAX_MAP_PORTALS		65536

// key / value pair sizes

#define	MAX_BSP_KEY		32
#define	MAX_BSP_VALUE	1024

//=============================================================================


#define BSPVERSION	29
#define	TOOLVERSION	2

typedef struct
{
	int		fileofs, filelen;
} lump_t;

#define	LUMP_ENTITIES	0
#define	LUMP_PLANES		1
#define	LUMP_TEXTURES	2
#define	LUMP_VERTEXES	3
#define	LUMP_VISIBILITY	4
#define	LUMP_NODES		5
#define	LUMP_TEXINFO	6
#define	LUMP_FACES		7
#define	LUMP_LIGHTING	8
#define	LUMP_CLIPNODES	9
#define	LUMP_LEAFS		10
#define	LUMP_MARKSURFACES 11
#define	LUMP_EDGES		12
#define	LUMP_SURFEDGES	13
#define	LUMP_MODELS		14

#define	HEADER_LUMPS	15

typedef struct
{
	float		mins[3], maxs[3];
	float		origin[3];
	int			headnode[MAX_MAP_HULLS];
	int			visleafs;		// not including the solid leaf 0
	int			firstface, numfaces;
} dmodel_t;

typedef struct
{
	int			version;	
	lump_t		lumps[HEADER_LUMPS];
} dheader_t;

typedef struct
{
	int			nummiptex;
	int			dataofs[4];		// [nummiptex]
} dmiptexlump_t;

#define	MIPLEVELS	4
typedef struct miptex_s
{
	char		name[16];
	unsigned	width, height;
	unsigned	offsets[MIPLEVELS];		// four mip maps stored
} miptex_t;


typedef struct
{
	float	point[3];
} dvertex_t;


// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5

typedef struct
{
	float	normal[3];
	float	dist;
	int		type;		// PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
} dplane_t;



#define	CONTENTS_EMPTY		-1
#define	CONTENTS_SOLID		-2
#define	CONTENTS_WATER		-3
#define	CONTENTS_SLIME		-4
#define	CONTENTS_LAVA		-5
#define	CONTENTS_SKY		-6
#define	CONTENTS_ORIGIN		-7		// removed at csg time
#define	CONTENTS_CLIP		-8		// changed to contents_solid

#define	CONTENTS_CURRENT_0		-9
#define	CONTENTS_CURRENT_90		-10
#define	CONTENTS_CURRENT_180	-11
#define	CONTENTS_CURRENT_270	-12
#define	CONTENTS_CURRENT_UP		-13
#define	CONTENTS_CURRENT_DOWN	-14


// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct
{
	int			planenum;
	short		children[2];	// negative numbers are -(leafs+1), not nodes
	short		mins[3];		// for sphere culling
	short		maxs[3];
	unsigned short	firstface;
	unsigned short	numfaces;	// counting both sides
} dnode_t;

typedef struct
{
	int			planenum;
	short		children[2];	// negative numbers are contents
} dclipnode_t;


typedef struct texinfo_s
{
	float		vecs[2][4];		// [s/t][xyz offset]
	int			miptex;
	int			flags;
} texinfo_t;
#define	TEX_SPECIAL		1		// sky or slime, no lightmap or 256 subdivision

// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct
{
	unsigned short	v[2];		// vertex numbers
} dedge_t;

#define	MAXLIGHTMAPS	4
typedef struct
{
	short		planenum;
	short		side;

	int			firstedge;		// we must support > 64k edges
	short		numedges;	
	short		texinfo;

// lighting info
	byte		styles[MAXLIGHTMAPS];
	int			lightofs;		// start of [numstyles*surfsize] samples
} dface_t;



#define	AMBIENT_WATER	0
#define	AMBIENT_SKY		1
#define	AMBIENT_SLIME	2
#define	AMBIENT_LAVA	3

#define	NUM_AMBIENTS			4		// automatic ambient sounds

// leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
// all other leafs need visibility info
typedef struct
{
	int			contents;
	int			visofs;				// -1 = no visibility info

	short		mins[3];			// for frustum culling
	short		maxs[3];

	unsigned short		firstmarksurface;
	unsigned short		nummarksurfaces;

	byte		ambient_level[NUM_AMBIENTS];
} dleaf_t;


//============================================================================

#ifndef QUAKE_GAME

#define	ANGLE_UP	-1
#define	ANGLE_DOWN	-2


// the utilities get to be lazy and just use large static arrays

EXTERN 	int			nummodels;
EXTERN 	dmodel_t	dmodels[MAX_MAP_MODELS];

EXTERN 	int			visdatasize;
EXTERN 	byte		dvisdata[MAX_MAP_VISIBILITY];

EXTERN 	int			lightdatasize;
EXTERN 	byte		dlightdata[MAX_MAP_LIGHTING];

EXTERN 	int			texdatasize;
EXTERN 	byte		dtexdata[MAX_MAP_MIPTEX]; // (dmiptexlump_t)

EXTERN 	int			entdatasize;
EXTERN 	char		dentdata[MAX_MAP_ENTSTRING];

EXTERN 	int			numleafs;
EXTERN 	dleaf_t		dleafs[MAX_MAP_LEAFS];

EXTERN 	int			numplanes;
EXTERN 	dplane_t	dplanes[MAX_MAP_PLANES];

EXTERN 	int			numvertexes;
EXTERN 	dvertex_t	dvertexes[MAX_MAP_VERTS];

EXTERN 	int			numnodes;
EXTERN 	dnode_t		dnodes[MAX_MAP_NODES];

EXTERN 	int			numtexinfo;
EXTERN 	texinfo_t	texinfo[MAX_MAP_TEXINFO];

EXTERN 	int			numfaces;
EXTERN 	dface_t		dfaces[MAX_MAP_FACES];

EXTERN 	int			numclipnodes;
EXTERN 	dclipnode_t	dclipnodes[MAX_MAP_CLIPNODES];

EXTERN 	int			numedges;
EXTERN 	dedge_t		dedges[MAX_MAP_EDGES];

EXTERN 	int			nummarksurfaces;
EXTERN 	unsigned short	dmarksurfaces[MAX_MAP_MARKSURFACES];

EXTERN 	int			numsurfedges;
EXTERN 	int			dsurfedges[MAX_MAP_SURFEDGES];


EXTERN_CPP void DecompressVis (byte *in, byte *decompressed);
EXTERN_CPP int CompressVis (byte *vis, byte *dest);

EXTERN_CPP void	LoadBSPFile (char *filename);
EXTERN_CPP void	WriteBSPFile (char *filename);
EXTERN_CPP void	PrintBSPFileSizes (void);

//===============


typedef struct epair_s
{
	struct epair_s	*next;
	char	*key;
	char	*value;
} epair_t;

typedef struct
{
	vec3_t		origin;
	int			firstbrush;
	int			numbrushes;
	epair_t		*epairs;
} entity_t;

EXTERN 	int			num_entities;
EXTERN 	entity_t	entities[MAX_MAP_ENTITIES];

EXTERN_CPP void	ParseEntities (void);
EXTERN_CPP void	UnparseEntities (void);

EXTERN_CPP void 	SetKeyValue (entity_t *ent, char *key, char *value);
EXTERN_CPP char 	*ValueForKey (entity_t *ent, char *key);
// will return "" if not present

EXTERN_CPP vec_t	FloatForKey (entity_t *ent, char *key);
EXTERN_CPP void 	GetVectorForKey (entity_t *ent, char *key, vec3_t vec);

EXTERN_CPP epair_t *ParseEpair (void);

#endif


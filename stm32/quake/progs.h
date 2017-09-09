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

#include "pr_comp.h"			// defs shared with qcc
#include "progdefs.h"			// generated by program cdefs

typedef union eval_s
{
	string_t		string;
	float			_float;
	float			vector[3];
	func_t			function;
	int				_int;
	int				edict;
} eval_t;	

#define	MAX_ENT_LEAFS	16
typedef struct edict_s
{
	qboolean	free;
	link_t		area;				// linked to a division node or leaf
	
	int			num_leafs;
	short		leafnums[MAX_ENT_LEAFS];

	entity_state_t	baseline;
	
	float		freetime;			// sv.time when the object was freed
	entvars_t	v;					// C exported fields from progs
// other fields from progs come immediately after
} edict_t;
#define	EDICT_FROM_AREA(l) STRUCT_FROM_LINK(l,edict_t,area)

//============================================================================

EXTERN 	dprograms_t		*progs;
EXTERN 	dfunction_t		*pr_functions;
EXTERN 	char			*pr_strings;
EXTERN 	ddef_t			*pr_globaldefs;
EXTERN 	ddef_t			*pr_fielddefs;
EXTERN 	dstatement_t	*pr_statements;
EXTERN 	globalvars_t	*pr_global_struct;
EXTERN 	float			*pr_globals;			// same as pr_global_struct

EXTERN 	int				pr_edict_size;	// in bytes

//============================================================================

EXTERN_CPP void PR_Init (void);

EXTERN_CPP void PR_ExecuteProgram (func_t fnum);
EXTERN_CPP void PR_LoadProgs (void);

EXTERN_CPP void PR_Profile_f (void);

EXTERN_CPP edict_t *ED_Alloc (void);
EXTERN_CPP void ED_Free (edict_t *ed);

EXTERN_CPP char	*ED_NewString (char *string);
// returns a copy of the string allocated from the server's string heap

EXTERN_CPP void ED_Print (edict_t *ed);
EXTERN_CPP void ED_Write (FILE *f, edict_t *ed);
EXTERN_CPP char *ED_ParseEdict (char *data, edict_t *ent);

EXTERN_CPP void ED_WriteGlobals (FILE *f);
EXTERN_CPP void ED_ParseGlobals (char *data);

EXTERN_CPP void ED_LoadFromFile (char *data);

//define EDICT_NUM(n) ((edict_t *)(sv.edicts+ (n)*pr_edict_size))
//define NUM_FOR_EDICT(e) (((byte *)(e) - sv.edicts)/pr_edict_size)

EXTERN_CPP edict_t *EDICT_NUM(int n);
EXTERN_CPP int NUM_FOR_EDICT(edict_t *e);

#define	NEXT_EDICT(e) ((edict_t *)( (byte *)e + pr_edict_size))

#define	EDICT_TO_PROG(e) ((byte *)e - (byte *)sv.edicts)
#define PROG_TO_EDICT(e) ((edict_t *)((byte *)sv.edicts + e))

//============================================================================

#define	G_FLOAT(o) (pr_globals[o])
#define	G_INT(o) (*(int *)&pr_globals[o])
#define	G_EDICT(o) ((edict_t *)((byte *)sv.edicts+ *(int *)&pr_globals[o]))
#define G_EDICTNUM(o) NUM_FOR_EDICT(G_EDICT(o))
#define	G_VECTOR(o) (&pr_globals[o])
#define	G_STRING(o) (pr_strings + *(string_t *)&pr_globals[o])
#define	G_FUNCTION(o) (*(func_t *)&pr_globals[o])

#define	E_FLOAT(e,o) (((float*)&e->v)[o])
#define	E_INT(e,o) (*(int *)&((float*)&e->v)[o])
#define	E_VECTOR(e,o) (&((float*)&e->v)[o])
#define	E_STRING(e,o) (pr_strings + *(string_t *)&((float*)&e->v)[o])

EXTERN 	int		type_size[8];

typedef void (*builtin_t) (void);
EXTERN 	builtin_t *pr_builtins;
EXTERN  int pr_numbuiltins;

EXTERN  int		pr_argc;

EXTERN 	qboolean	pr_trace;
EXTERN 	dfunction_t	*pr_xfunction;
EXTERN 	int			pr_xstatement;

EXTERN 	unsigned short		pr_crc;

EXTERN_CPP void PR_RunError (char *error, ...);

EXTERN_CPP void ED_PrintEdicts (void);
EXTERN_CPP void ED_PrintNum (int ent);

EXTERN_CPP eval_t *GetEdictFieldValue(edict_t *ed, char *field);


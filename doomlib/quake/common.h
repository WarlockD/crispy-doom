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
// comndef.h  -- general definitions

#if !defined BYTE_DEFINED
using byte = unsigned char;
#define BYTE_DEFINED 1
#endif

// ok fuck this, lets use stdbool
using  qboolean = bool;

#ifndef EXTERN_CPP
//#ifdef __cplusplus
#if 0
#define EXTERN_CPP extern "C"
#define EXTERN extern "C"
#else
#define EXTERN extern
#define EXTERN_CPP
#endif
#endif


//============================================================================

typedef struct sizebuf_s
{
	qboolean	allowoverflow;	// if false, do a Sys_Error
	qboolean	overflowed;		// set to true if the buffer size failed
	byte	*data;
	int		maxsize;
	int		cursize;
} sizebuf_t;

EXTERN_CPP void SZ_Alloc (sizebuf_t *buf, int startsize);
EXTERN_CPP void SZ_Free (sizebuf_t *buf);
EXTERN_CPP void SZ_Clear (sizebuf_t *buf);
EXTERN_CPP void *SZ_GetSpace (sizebuf_t *buf, int length);
EXTERN_CPP void SZ_Write (sizebuf_t *buf, void *data, int length);
EXTERN_CPP void SZ_Print (sizebuf_t *buf, char *data);	// strcats onto the sizebuf

//============================================================================

typedef struct link_s
{
	struct link_s	*prev, *next;
} link_t;


EXTERN_CPP void ClearLink (link_t *l);
EXTERN_CPP void RemoveLink (link_t *l);
EXTERN_CPP void InsertLinkBefore (link_t *l, link_t *before);
EXTERN_CPP void InsertLinkAfter (link_t *l, link_t *after);

// (type *)STRUCT_FROM_LINK(link_t *link, type, member)
// ent = STRUCT_FROM_LINK(link,entity_t,order)
// FIXME: remove this mess!
#define	STRUCT_FROM_LINK(l,t,m) ((t *)((byte *)l - (int)&(((t *)0)->m)))

//============================================================================

#ifndef NULL
#define NULL ((void *)0)
#endif

#define Q_MAXCHAR ((char)0x7f)
#define Q_MAXSHORT ((short)0x7fff)
#define Q_MAXINT	((int)0x7fffffff)
#define Q_MAXLONG ((int)0x7fffffff)
#define Q_MAXFLOAT ((int)0x7fffffff)

#define Q_MINCHAR ((char)0x80)
#define Q_MINSHORT ((short)0x8000)
#define Q_MININT 	((int)0x80000000)
#define Q_MINLONG ((int)0x80000000)
#define Q_MINFLOAT ((int)0x7fffffff)

//============================================================================

EXTERN 	qboolean		bigendien;

EXTERN 	short	(*BigShort) (short l);
EXTERN 	short	(*LittleShort) (short l);
EXTERN 	int	(*BigLong) (int l);
EXTERN 	int	(*LittleLong) (int l);
EXTERN 	float	(*BigFloat) (float l);
EXTERN 	float	(*LittleFloat) (float l);

//============================================================================

EXTERN_CPP void MSG_WriteChar (sizebuf_t *sb, int c);
EXTERN_CPP void MSG_WriteByte (sizebuf_t *sb, int c);
EXTERN_CPP void MSG_WriteShort (sizebuf_t *sb, int c);
EXTERN_CPP void MSG_WriteLong (sizebuf_t *sb, int c);
EXTERN_CPP void MSG_WriteFloat (sizebuf_t *sb, float f);
EXTERN_CPP void MSG_WriteString (sizebuf_t *sb, char *s);
EXTERN_CPP void MSG_WriteCoord (sizebuf_t *sb, float f);
EXTERN_CPP void MSG_WriteAngle (sizebuf_t *sb, float f);

EXTERN 	int			msg_readcount;
EXTERN 	qboolean	msg_badread;		// set if a read goes beyond end of message

EXTERN_CPP void MSG_BeginReading (void);
EXTERN_CPP int MSG_ReadChar (void);
EXTERN_CPP int MSG_ReadByte (void);
EXTERN_CPP int MSG_ReadShort (void);
EXTERN_CPP int MSG_ReadLong (void);
EXTERN_CPP float MSG_ReadFloat (void);
EXTERN_CPP char *MSG_ReadString (void);

EXTERN_CPP float MSG_ReadCoord (void);
EXTERN_CPP float MSG_ReadAngle (void);

//============================================================================

EXTERN_CPP void Q_memset (void *dest, int fill, int count);
EXTERN_CPP void Q_memcpy (void *dest, void *src, int count);
EXTERN_CPP int Q_memcmp (void *m1, void *m2, int count);
EXTERN_CPP void Q_strcpy (char *dest, char *src);
EXTERN_CPP void Q_strncpy (char *dest, char *src, int count);
EXTERN_CPP int Q_strlen (char *str);
EXTERN_CPP char *Q_strrchr (char *s, char c);
EXTERN_CPP void Q_strcat (char *dest, char *src);
EXTERN_CPP int Q_strcmp (char *s1, char *s2);
EXTERN_CPP int Q_strncmp (char *s1, char *s2, int count);
EXTERN_CPP int Q_strcasecmp (char *s1, char *s2);
EXTERN_CPP int Q_strncasecmp (char *s1, char *s2, int n);
EXTERN_CPP int	Q_atoi (char *str);
EXTERN_CPP float Q_atof (char *str);

//============================================================================

EXTERN 	char		com_token[1024];
EXTERN 	qboolean	com_eof;

EXTERN_CPP char *COM_Parse (char *data);


EXTERN 	int		com_argc;
EXTERN 	char	**com_argv;

EXTERN_CPP int COM_CheckParm (char *parm);
EXTERN_CPP void COM_Init (char *path);
EXTERN_CPP void COM_InitArgv (int argc, char **argv);

EXTERN_CPP char *COM_SkipPath (char *pathname);
EXTERN_CPP void COM_StripExtension (char *in, char *out);
EXTERN_CPP void COM_FileBase (char *in, char *out);
EXTERN_CPP void COM_DefaultExtension (char *path, char *extension);

EXTERN_CPP char	*va(char *format, ...);
// does a varargs printf into a temp buffer


//============================================================================

EXTERN  int com_filesize;
struct cache_user_s;

EXTERN 	char	com_gamedir[MAX_OSPATH];

EXTERN_CPP void COM_WriteFile (char *filename, void *data, int len);
EXTERN_CPP int COM_OpenFile (char *filename, int *hndl);
EXTERN_CPP int COM_FOpenFile (char *filename, FILE **file);
EXTERN_CPP void COM_CloseFile (int h);

EXTERN_CPP byte *COM_LoadStackFile (char *path, void *buffer, int bufsize);
EXTERN_CPP byte *COM_LoadTempFile (char *path);
EXTERN_CPP byte *COM_LoadHunkFile (char *path);
EXTERN_CPP void COM_LoadCacheFile (char *path, struct cache_user_s *cu);


EXTERN 	struct cvar_s	registered;

EXTERN  qboolean		standard_quake, rogue, hipnotic;

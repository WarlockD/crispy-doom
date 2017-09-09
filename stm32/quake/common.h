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

namespace quake {
	using byte = uint8_t;
	void Q_memset (void *dest, int fill, int count);
	void Q_memcpy (void *dest, const void *src, int count);
	int Q_memcmp (const void *m1, const void *m2, int count);
	void Q_strcpy (char *dest, const char *src);
	void Q_strncpy (char *dest, const char *src, int count);
	int Q_strlen (const char *str);
	char *Q_strrchr(char *s, char c);
	void Q_strcat (char *dest, const char *src);
	int Q_strcmp (const char *s1, const char *s2);
	int Q_strncmp (const char *s1, const char *s2, int count);
	int Q_strncasecmp (const char *s1, const char *s2, int n);
	int Q_strcasecmp (const char *s1, const char *s2);
	int Q_atoi (const char *str);
	float Q_atof (const char *str);

	struct sizebuf_t
	{
		bool	allowoverflow;	// if false, do a Sys_Error
		bool	overflowed;		// set to true if the buffer size failed
		byte	*data;
		int		maxsize;
		int		cursize;
		int     msg_readcount;
		bool    msg_badread;
		sizebuf_t(int startsize)
		void SZ_Alloc ( int startsize);
		void SZ_Free ();
		void SZ_Clear ();
		void *SZ_GetSpace ( int length);
		void SZ_Write (void *data, int length);
		void SZ_Print (char *data);	// strcats onto the sizebuf
		void WriteChar (int c);
		void WriteByte ( int c);
		void WriteShort (int c);
		void WriteLong (int c);
		void WriteFloat ( float f);
		void WriteString (const char *s);
		void WriteCoord (float f);
		void WriteAngle (float f);
	} ;



	EXTERN_CPP void SZ_Alloc (sizebuf_t *buf, int startsize);
	EXTERN_CPP void SZ_Free (sizebuf_t *buf);
	EXTERN_CPP void SZ_Clear (sizebuf_t *buf);
	EXTERN_CPP void *SZ_GetSpace (sizebuf_t *buf, int length);
	EXTERN_CPP void SZ_Write (sizebuf_t *buf, void *data, int length);
	EXTERN_CPP void SZ_Print (sizebuf_t *buf, char *data);	// strcats onto the sizebuf

	//============================================================================

	struct link_t
	{
		link_t	*prev, *next;

		link_t() : prev(nullptr), next(nullptr) {}// ClearLink is used for new headnodes
		void ClearLink() { prev = next = nullptr; }

		void RemoveLink () { if(next) { next->prev = prev; prev->next = next; } }
		void InsertLinkBefore (link_t *before)
		{
			next = before;
			prev = before->prev;
			prev->next = this;
			next->prev = this;
		}
		void InsertLinkAfter (link_t *after)
		{
			next = after->next;
			prev = after;
			prev->next = this;
			next->prev = this;
		}
	} ;
	struct common_t {
#if CPP_14
		template<class T>
		constexpr typename std::enable_if<std::is_unsigned<T>::value, T>::type
		bswap(T i, T j = 0u, std::size_t n = 0u) {
		  return n == sizeof(T) ? j :
		    bswap<T>(i >> CHAR_BIT, (j << CHAR_BIT) | (i & (T)(unsigned char)(-1)), n + 1);
		}
#else
		template<typename T, std::size_t... N>
		constexpr T bswap_impl(T i, std::index_sequence<N...>) {
		  return ((((i >> (N * CHAR_BIT)) & (T)(unsigned char)(-1)) <<
		           ((sizeof(T) - 1 - N) * CHAR_BIT)) | ...);
		}; //                                        ^~~~~ fold expression
		template<typename T>
		typename std::enable_if<std::is_integral<E>::value,T>::type
		constexpr bswap(T i) {
			using unsigned_type = typename std::make_unsigned<T>::type;
			return bswap_impl<unsigned_type>(i, std::make_index_sequence<sizeof(T)>{});
		}
#endif
		static short ShortSwap (short l)
		{
			byte    b1,b2;

			b1 = l&255;
			b2 = (l>>8)&255;

			return (b1<<8) + b2;
		}

		short   ShortNoSwap (short l)
		{
			return l;
		}

		int    LongSwap (int l)
		{
			byte    b1,b2,b3,b4;

			b1 = l&255;
			b2 = (l>>8)&255;
			b3 = (l>>16)&255;
			b4 = (l>>24)&255;

			return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
		}

		int     LongNoSwap (int l)
		{
			return l;
		}

		float FloatSwap (float f)
		{
			union
			{
				float   f;
				byte    b[4];
			} dat1, dat2;


			dat1.f = f;
			dat2.b[0] = dat1.b[3];
			dat2.b[1] = dat1.b[2];
			dat2.b[2] = dat1.b[1];
			dat2.b[3] = dat1.b[0];
			return dat2.f;
		}

		float FloatNoSwap (float f)
		{
			return f;
		}

		inline short	BigShort (short l) const { return _BigShort(l); }
		inline short	LittleShort (short l){ return _LittleShort(l); }
		inline int		BigLong (int l){ return _BigLong(l); }
		inline int		LittleLong (int l){ return _LittleLong(l); }
		inline float 	BigFloat (float l){ return _BigFloat(l); }
		inline float 	LittleFloat (float l){ return _LittleFloat(l); }
		common_t();
		bool IsBigEndien() const { return _bigedien; }
	private:
		bool _bigedien;
		short	(*_BigShort) (short l);
		short	(*_LittleShort) (short l);
		int		(*_BigLong) (int l);
		int		(*_LittleLong) (int l);
		float 	(*_BigFloat) (float l);
		float 	(*_LittleFloat) (float l);
	};





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
};

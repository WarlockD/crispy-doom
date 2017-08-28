#pragma once
/*
Copyright(C) 1996 - 1997 Id Software, Inc.
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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111 - 1307, USA.
* /
/*
memory allocation
H_??? The hunk manages the entire memory block given to quake.  It must be
contiguous.  Memory can be allocated from either the low or high end in a
stack fashion.  The only way memory is released is by resetting one of the
pointers.
Hunk allocations should be given a name, so the Hunk_Print () function
can display usage.
Hunk allocations are guaranteed to be 16 byte aligned.
The video buffers are allocated high to avoid leaving a hole underneath
server allocations when changing to a higher video mode.
Z_??? Zone memory functions used for small, dynamic allocations like text
strings from command input.  There is only about 48K for it, allocated at
the very bottom of the hunk.
Cache_??? Cache memory is for objects that can be dynamically loaded and
can usefully stay persistant between levels.  The size of the cache
fluctuates from level to level.
To allocate a cachable object
Temp_??? Temp memory is used for file loading and surface caching.  The size
of the cache memory is adjusted so that there is a minimum of 512k remaining
for temp memory.
------ Top of Memory -------
high hunk allocations
<--- high hunk reset point held by vid
video buffer
z buffer
surface cache
<--- high hunk used
cachable memory
<--- low hunk used
client and server low hunk allocations
<-- low hunk reset point held by host
startup hunk allocations
Zone block
----- Bottom of Memory -----
*/
#include <cstdint>
#include <type_traits>
#include <memory>
namespace quake {
	// define the memory set and use functions here
	static inline void Q_memset(void* ptr, int value, size_t size) { ::memset(ptr, value, size); }
	static inline void Q_strncpy(char* l, const char* r, size_t size) { ::strncpy(l, r, size); }
	
	//static inline void Q_memset(void* ptr, int value, size_t size) { ::memset(ptr, value, size); }
	class Memory {
		static constexpr size_t DYNAMIC_SIZE = 0xc000;
		static constexpr uint32_t ZONEID = 0x1d4a11;
		static constexpr size_t MINFRAGMENT = 64;
		struct memblock_t
		{
			uint32_t	size;           // including the header and possibly tiny fragments
			uint32_t	tag;            // a tag of 0 is a free block
			uint32_t	id;        		// should be ZONEID
			memblock_t	*next, *prev;
			uint32_t	pad;			// pad to 64 bit boundary
		} ;

		struct memzone_t
		{
			uint32_t	size;		// total bytes malloced, including header
			memblock_t	blocklist;		// start / end cap for linked list
			memblock_t	*rover;
			void Print();
			memzone_t(uint32_t size);
		} ;

	public:
		Memory(void* buf, size_t size);
		void Free(void* ptr);
		void* Malloc(size_t size);
		void* Malloc(size_t size, uint32_t tag);
		void DumpHeap();
		void CheckHeap();
		void FreeMemory();
		
	private:
		memzone_t* mainzone;
	};



	class Hunk {
		static constexpr uint32_t HUNK_SENTINAL = 0x1df001ed;
		typedef struct
		{
			int		sentinal;
			int		size;		// including sizeof(hunk_t), -1 = not allocated
			char	name[8];
		} hunk_t;
	public:
		void *Alloc(size_t size);		// returns 0 filled memory
		void *AllocName(size_t size, const char *name);

		void *HighAllocName(size_t size, const char *name);

		int	LowMark();
		void FreeToLowMark(int mark);

		int	HighMark();
		void FreeToHighMark(int mark);

		void *TempAlloc(size_t size);

		void Check();
		void Print(bool all);
		typedef struct cache_user_s
		{
			void	*data;
		} cache_user_t;

		void Cache_Flush(void);

		void *Cache_Check(cache_user_t *c);
		// returns the cached data, and moves to the head of the LRU list
		// if present, otherwise returns NULL

		void Cache_Free(cache_user_t *c);

		void *Cache_Alloc(cache_user_t *c, int size, char *name);
		// Returns NULL if all purgable data was tossed and there still
		// wasn't enough room.

		void Cache_Report(void);
	private:
		uint8_t	*hunk_base;
		int		hunk_size;

		int		hunk_low_used;
		int		hunk_high_used;

		bool	hunk_tempactive;
		int		hunk_tempmark;
	};





	void Memory_Init(void *buf, int size);

	void Z_Free(void *ptr);
	void *Z_Malloc(int size);			// returns 0 filled memory
	void *Z_TagMalloc(int size, int tag);

	void Z_DumpHeap(void);
	void Z_CheckHeap(void);
	int Z_FreeMemory(void);



}
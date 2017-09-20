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
#ifndef _ZONE_H_
#define _ZONE_H_

#include <vector>
#include <string>
#include <sstream>
#include <memory>

EXTERN_CPP void Memory_Init (void *buf, int size);

EXTERN_CPP void Z_Free (void *ptr);
EXTERN_CPP void *Z_Malloc (int size);			// returns 0 filled memory
EXTERN_CPP void *Z_TagMalloc (int size, int tag);

EXTERN_CPP void Z_DumpHeap (void);
EXTERN_CPP void Z_CheckHeap (void);
EXTERN_CPP int Z_FreeMemory (void);

EXTERN_CPP void *Hunk_Alloc (int size);		// returns 0 filled memory
EXTERN_CPP void *Hunk_AllocName (int size, const char *name);

EXTERN_CPP void *Hunk_HighAllocName (int size, const char *name);

EXTERN_CPP int	Hunk_LowMark (void);
EXTERN_CPP void Hunk_FreeToLowMark (int mark);

EXTERN_CPP int	Hunk_HighMark (void);
EXTERN_CPP void Hunk_FreeToHighMark (int mark);

EXTERN_CPP void *Hunk_TempAlloc (int size);

EXTERN_CPP void Hunk_Check (void);

typedef struct cache_user_s
{
	void	*data;
} cache_user_t;

EXTERN_CPP void Cache_Flush (void);

EXTERN_CPP void *Cache_Check (cache_user_t *c);
// returns the cached data, and moves to the head of the LRU list
// if present, otherwise returns NULL

EXTERN_CPP void Cache_Free (cache_user_t *c);

EXTERN_CPP void *Cache_Alloc (cache_user_t *c, int size, char *name);
// Returns NULL if all purgable data was tossed and there still
// wasn't enough room.

EXTERN_CPP void Cache_Report (void);


template <typename T>
class zmalloc_allocator {
public:
	typedef size_t size_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	template<typename _Tp1>
	struct rebind
	{
		typedef zmalloc_allocator<_Tp1> other;
	};
	pointer allocate(size_type n, const void *hint = nullptr)
	{
		//fprintf(stderr, "Alloc %d bytes.\n", n * sizeof(T));
		return (pointer*)Z_Malloc(n);
	}

	void deallocate(pointer p, size_type n)
	{
		(void)n;
		Z_Free(p);
		//fprintf(stderr, "Dealloc %d bytes (%p).\n", n * sizeof(T), p);
	}
	zmalloc_allocator() { }
	zmalloc_allocator(const zmalloc_allocator &a) { }
	template <class U>
	zmalloc_allocator(const zmalloc_allocator<U> &a) { }
	~zmalloc_allocator() throw() { }
};

struct ZMemoryObject {
	static inline void* operator new(size_t size) {
		//fprintf(stderr, "Alloc %d bytes.\n", n * sizeof(T));
		return (void*)Z_Malloc(size);
	}
	static inline void* operator new(size_t size, int tag) {
		//fprintf(stderr, "Alloc %d bytes.\n", n * sizeof(T));
		return (void*)Z_TagMalloc(size, tag);
	}
	static inline void operator delete(void* ptr) {
		//fprintf(stderr, "Alloc %d bytes.\n", n * sizeof(T));
		Z_Free(ptr);
	}
};
struct HunkObject {
	static inline void* operator new(size_t size) {
		//fprintf(stderr, "Alloc %d bytes.\n", n * sizeof(T));
		return (void*)Hunk_Alloc(n);
	}
	static inline  void* operator new(size_t size, const char* name) {
		//fprintf(stderr, "Alloc %d bytes.\n", n * sizeof(T));
		return (void*)Hunk_AllocName(size, name);
	}
	static inline  void operator delete(void* ptr) {
		(void)ptr, size;

	}
};
template <typename T>
class zhunk_allocator {
public:
	typedef size_t size_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	template<typename _Tp1>
	struct rebind
	{
		typedef zhunk_allocator<_Tp1> other;
	};
	pointer allocate(size_type n, const void *hint = nullptr)
	{
		//fprintf(stderr, "Alloc %d bytes.\n", n * sizeof(T));
		return (pointer*)Hunk_Alloc(n);
	}

	void deallocate(pointer p, size_type n)
	{
		(void)n;
		//Z_Free(p);
		//fprintf(stderr, "Dealloc %d bytes (%p).\n", n * sizeof(T), p);
	}
	zhunk_allocator() { }
	zhunk_allocator(const zhunk_allocator &a) { }
	template <class U>
	zhunk_allocator(const zhunk_allocator<U> &a) { }
	~zhunk_allocator() throw() { }
};

template<typename T>
// hunks are usally taken care of at the end of a level
template<typename T>
class HunkData {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using refrence = T&;
	using const_refrence = const T&;
	HunkData() : _data(nullptr) { std::unique_ptr}
	HunkData(size_t size, const char* name) : _data((pointer)Hunk_AllocName((int)size, name)) {}
	HunkData(size_t size) : _data((pointer)Hunk_Alloc((int)size)) {}
	~HunkData() { _data = nullptr; }
	refrence operator*() { return *_data; }
	const_refrence operator*() const { return *_data; }
	pointer operator->() { return _data; }
	const_pointer operator>() const { return _data; }
	pointer data() { return _data; }
	const_pointer data() const { return _data; }

	template<typename TT>
	bool operator==(const HunkData<TT>& r) const { return (void*)data() == (void*)r.data(); }
	template<typename TT>
	bool operator!=(const HunkData<TT>& r) const { return (void*)data() != (void*)r.data(); }
private:
	pointer _data;
};




using qstring = std::basic_string<char, std::char_traits<char>, zmalloc_allocator<char>>;
using qstring_view_t = std::string_view;

template<typename T>
using qvector = std::vector<T, zmalloc_allocator<T>>;

#endif // _ZONE_H_


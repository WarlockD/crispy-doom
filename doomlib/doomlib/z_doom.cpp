#pragma once
#include "z_doom.hpp"

#define Z_MALLOC_DEBUG

namespace doom_cpp{

	//
	// ZONE MEMORY ALLOCATION
	//
	// There is never any space between memblocks,
	//  and there will never be two contiguous free memblocks.
	// The rover can be left pointing at a non-empty block.
	//
	// It is of no value to free a cachable block,
	//  because it will get overwritten automatically if needed.
	// 
	static constexpr size_t MEM_ALIGN = sizeof(void *);
	static constexpr uint32_t ZONEID = 0x1d4a11;
	static constexpr size_t MINFRAGMENT = 64;
#ifdef _WIN32
#ifdef _MSC_VER /* not needed for Visual Studio 2008 */
#pragma pack(push,1)
	struct memblock_t
	{
		size_t		size;	// including the header and possibly tiny fragments
		void**		user;
		PU			tag;	// PU_FREE if this is free
		uint32_t	id;	// should be ZONEID
		memblock_t*	next;
		memblock_t*	prev;
	};
	struct memzone_t
	{
		// total bytes malloced, including header
		int		size;
		// start / end cap for linked list
		memblock_t	blocklist;

		memblock_t*	rover;

	};
#pragma pack(pop)
#endif
#endif
	static memzone_t *mainzone = nullptr;
	static bool zero_on_free = false;
	static bool scan_on_free = false;


	//
	// Z_ClearZone
	//
	void Z_ClearZone(memzone_t* zone)
	{
		memblock_t*		block;

		// set the entire zone to one free block
		zone->blocklist.next =
			zone->blocklist.prev =
			block = (memblock_t *)((uint8_t *)zone + sizeof(memzone_t));

		zone->blocklist.user = (void **)zone;
		zone->blocklist.tag = PU::STATIC;
		zone->rover = block;

		block->prev = block->next = &zone->blocklist;

		// a free block.
		block->tag = PU::FREE;

		block->size = zone->size - sizeof(memzone_t);
	}


	void	Z_Init(void* memory_base, size_t size) {
		memblock_t*	block;

		mainzone = (memzone_t *)memory_base;
		mainzone->size = size;

		// set the entire zone to one free block
		mainzone->blocklist.next =
			mainzone->blocklist.prev =
			block = (memblock_t *)((uint8_t *)mainzone + sizeof(memzone_t));

		mainzone->blocklist.user = (void **)mainzone;
		mainzone->blocklist.tag = PU::STATIC;
		mainzone->rover = block;

		block->prev = block->next = &mainzone->blocklist;

		// free block
		block->tag = PU::FREE;

		block->size = mainzone->size - sizeof(memzone_t);

		
		// [Deliberately undocumented]
		// Zone memory debugging flag. If set, memory is zeroed after it is freed
		// to deliberately break any code that attempts to use it after free.
		//
		zero_on_free = M_ParmExists("-zonezero");

		// [Deliberately undocumented]
		// Zone memory debugging flag. If set, each time memory is freed, the zone
		// heap is scanned to look for remaining pointers to the freed block.
		//
		scan_on_free = M_ParmExists("-zonescan");
	}


	// Scan the zone heap for pointers within the specified range, and warn about
	// any remaining pointers.
	static void ScanForBlock(void *start, void *end)
	{
		memblock_t *block;
		void **mem;
		PU tag;
		int i, len;

		block = mainzone->blocklist.next;

		while (block->next != &mainzone->blocklist)
		{
			tag = block->tag;

			if (tag == PU::STATIC || tag == PU::LEVEL || tag == PU::LEVSPEC)
			{
				// Scan for pointers on the assumption that pointers are aligned
				// on word boundaries (word size depending on pointer size):
				mem = (void **)((uint8_t *)block + sizeof(memblock_t));
				len = (block->size - sizeof(memblock_t)) / sizeof(void *);

				for (i = 0; i < len; ++i)
				{
					if (start <= mem[i] && mem[i] <= end)
					{
						fprintf(stderr,
							"%p has dangling pointer into freed block "
							"%p (%p -> %p)\n",
							mem, start, &mem[i], mem[i]);
					}
				}
			}

			block = block->next;
		}
	}

	//
	// Z_Free
	//
	void Z_Free(void* ptr)
	{
		memblock_t*		block;
		memblock_t*		other;

		block = (memblock_t *)((uint8_t *)ptr - sizeof(memblock_t));

		if (block->id != ZONEID)
			I_Error("Z_Free: freed a pointer without ZONEID");

		if (block->tag != PU::FREE && block->user != NULL)
		{
			// clear the user's mark
			*block->user = 0;
		}

		// mark as free
		block->tag = PU::FREE;
		block->user = nullptr;
		block->id = 0;

		// If the -zonezero flag is provided, we zero out the block on free
		// to break code that depends on reading freed memory.
		if (zero_on_free) 	memset(ptr, 0, block->size - sizeof(memblock_t));
		if (scan_on_free) 	ScanForBlock(ptr, (uint8_t *)ptr + block->size - sizeof(memblock_t));

		other = block->prev;

		if (other->tag == PU::FREE)
		{
			// merge with previous free block
			other->size += block->size;
			other->next = block->next;
			other->next->prev = other;

			if (block == mainzone->rover)
				mainzone->rover = other;

			block = other;
		}

		other = block->next;
		if (other->tag == PU::FREE)
		{
			// merge the next free block onto the end
			block->size += other->size;
			block->next = other->next;
			block->next->prev = block;

			if (other == mainzone->rover)
				mainzone->rover = block;
		}
	}

	void* Z_Malloc(size_t size, PU tag, void** user)
	{
		printf("Z_Malloc: begin\r\n");
		Z_CheckHeap();
		Z_FileDumpHeap(stderr);
		int		extra;
		memblock_t*	start;
		memblock_t* rover;
		memblock_t* newblock;
		memblock_t*	base;
		void *result;
		assert(size > 0 && size < mainzone->size); // some checks
												   

		size += sizeof(memblock_t); // account for size of block header
		size = (size + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);

		// scan through the block list,
		// looking for the first free block
		// of sufficient size,
		// throwing out any purgable blocks along the way.



		// if there is a free block behind the rover,
		//  back up over them
		base = mainzone->rover;

		if (base->prev->tag == PU::FREE)
			base = base->prev;

		rover = base;
		start = base->prev;

		do
		{
			if (rover == start)
			{
				// scanned all the way around the list

				I_Error("Z_Malloc: failed on allocation of %i bytes, free memory %i", size, Z_FreeMemory());
			}

			if (rover->tag != PU::FREE)
			{
				if (rover->tag < PU::PURGELEVEL)
				{
					// hit a block that can't be purged,
					// so move base past it
					base = rover = rover->next;
				}
				else
				{
					// free the rover block (adding the size to base)

					// the rover can be the base block
					base = base->prev;
					Z_Free((uint8_t *)rover + sizeof(memblock_t));
					base = base->next;
					rover = base->next;
				}
			}
			else
			{
				rover = rover->next;
			}

		} while (base->tag != PU::FREE || base->size < size);


		// found a block big enough
		extra = base->size - size;

		if (extra >  MINFRAGMENT)
		{
			// there will be a free fragment after the allocated block
			newblock = (memblock_t *)(((uint8_t *)base) + size);
			newblock->size = extra;

			newblock->tag = PU::FREE;
			newblock->user = NULL;
			newblock->prev = base;
			newblock->next = base->next;
			newblock->next->prev = newblock;

			base->next = newblock;
			base->size = size;
		}

		if (user == NULL && tag >= PU::PURGELEVEL)
			I_Error("Z_Malloc: an owner is required for purgable blocks");

		base->user = (void**)user;
		base->tag = tag;

		result = (void *)((uint8_t *)base + sizeof(memblock_t));

		if (base->user) *base->user = result;

		// next allocation will start looking here
		mainzone->rover = base->next;

		base->id = ZONEID;
		printf("Z_Malloc: end\r\n");
		Z_CheckHeap();
		Z_FileDumpHeap(stderr);
		return result;
	}


	//
	// Z_FreeTags
	//
	void Z_FreeTags(PU		lowtag,PU		hightag)
	{
		memblock_t*	block;
		memblock_t*	next;
		for (block = mainzone->blocklist.next;
			block != &mainzone->blocklist;
			block = next)
		{
			// get link before freeing
			next = block->next;

			// free block?
			if (block->tag == PU::FREE)
				continue;

			if (static_cast<uint32_t>(block->tag) >= static_cast<uint32_t>(lowtag) && 
				static_cast<uint32_t>(block->tag) <= static_cast<uint32_t>(hightag))
				Z_Free((uint8_t *)block + sizeof(memblock_t));
		}
	}

	const char* Z_TagToString(PU tag) {
		const char* _tostring[] = {
			"PU::BAD",
			"PU::STATIC",
			"PU::SOUND",
			"PU::MUSIC",
			"PU::FREE",
			"PU::LEVEL",
			"PU::LEVSPEC",
			"PU::PURGELEVEL",
			"PU::CACHE",
			"PU::NUM_TAGS",
		};
		return _tostring[static_cast<uint32_t>(tag)];
	}

	//
	// Z_DumpHeap
	// Note: TFileDumpHeap( stdout ) ?
	//
	void Z_DumpHeap(PU		lowtag,PU		hightag)
	{
		memblock_t*	block;

		printf("zone size: %i  location: %p\n",
			mainzone->size, mainzone);

		printf("tag range: %i to %i\n",
			lowtag, hightag);

		for (block = mainzone->blocklist.next; ; block = block->next)
		{
			if (block->tag >= lowtag && block->tag <= hightag)
				printf("block:%p    size:%7i    user:%p    tag:%s\n" // %3i \n"
					, block, block->size, block->user, Z_TagToString(block->tag));

			if (block->next == &mainzone->blocklist)
			{
				// all blocks have been hit
				break;
			}

			if ((uint8_t *)block + block->size != (uint8_t *)block->next)
				printf("ERROR: block size does not touch the next block\n");

			if (block->next->prev != block)
				printf("ERROR: next block doesn't have proper back link\n");

			if (block->tag == PU::FREE && block->next->tag == PU::FREE)
				printf("ERROR: two consecutive free blocks\n");
		}
	}



	//
	// Z_FileDumpHeap
	//
	void Z_FileDumpHeap(FILE* f)
	{
		memblock_t*	block;

		fprintf(f, "zone size: %i  location: %p\n", mainzone->size, mainzone);

		for (block = mainzone->blocklist.next; ; block = block->next)
		{
			fprintf(f, "block:%p    size:%7i    user:%p    tag:%s\n",
				block, block->size, block->user, Z_TagToString(block->tag)); 

			if (block->next == &mainzone->blocklist)
			{
				// all blocks have been hit
				break;
			}

			if (((uint8_t *)block + block->size) != (uint8_t *)block->next)
				fprintf(f, "ERROR: block size does not touch the next block\n");

			if (block->next->prev != block)
				fprintf(f, "ERROR: next block doesn't have proper back link\n");

			if (block->tag == PU::FREE && block->next->tag == PU::FREE)
				fprintf(f, "ERROR: two consecutive free blocks\n");
		}
	}



	//
	// Z_CheckHeap
	//
	void Z_CheckHeap(void)
	{
		memblock_t*	block;

		for (block = mainzone->blocklist.next; ; block = block->next)
		{
			if (block->next == &mainzone->blocklist)
			{
				// all blocks have been hit
				break;
			}

			if ((uint8_t *)block + block->size != (uint8_t *)block->next)
				I_Error("Z_CheckHeap: block size does not touch the next block\n");

			if (block->next->prev != block)
				I_Error("Z_CheckHeap: next block doesn't have proper back link\n");

			if (block->tag == PU::FREE && block->next->tag == PU::FREE)
				I_Error("Z_CheckHeap: two consecutive free blocks\n");
		}
	}




	//
	// Z_ChangeTag
	//
	void Z_ChangeTag2(void *ptr, PU tag,const  char *file, size_t line)
	{
		memblock_t*	block;

		block = (memblock_t *)((uint8_t *)ptr - sizeof(memblock_t));

		if (block->id != ZONEID)
			I_Error("%s:%i: Z_ChangeTag: block without a ZONEID!",
				file, line);

		if (tag >= PU::PURGELEVEL && block->user == NULL)
			I_Error("%s:%i: Z_ChangeTag: an owner is required "
				"for purgable blocks", file, line);

		block->tag = tag;
	}

	void Z_ChangeUser(void *ptr, void **user)
	{
		memblock_t*	block;

		block = (memblock_t *)((uint8_t *)ptr - sizeof(memblock_t));

		if (block->id != ZONEID)
		{
			I_Error("Z_ChangeUser: Tried to change user for invalid block!");
		}

		block->user = user;
		*user = ptr;
	}


	//
	// Z_FreeMemory
	//
	size_t Z_FreeMemory()
	{
		size_t			free = 0;

		for (memblock_t* block = mainzone->blocklist.next;
			block != &mainzone->blocklist;
			block = block->next)
		{
			if (block->tag == PU::FREE || block->tag >= PU::PURGELEVEL)
				free += block->size;
		}

		return free;
	}
	size_t Z_UsedMemory() { 
		return Z_FreeMemory();
	}

	size_t Z_ZoneSize()
	{
		return mainzone->size;
	}

	void * doom_object::operator new (size_t size) {
		return Z_Malloc(size, PU::STATIC, nullptr);
	}
	void doom_object::operator delete (void * mem) {
		Z_Free(mem);
	}
	void *  doom_object::operator new (size_t size, PU tag) {
		return Z_Malloc(size, tag, nullptr);
	}
	void *  doom_object::operator new (size_t size, PU tag, void** user) {
		return Z_Malloc(size, tag, user);
	}

}
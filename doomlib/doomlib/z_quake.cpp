#include "z_quake.h"

namespace quake {
	void Sys_Error(const char* fmt, ...);
	void Con_Printf(const char* fmt, ...);
	void Memory::memzone_t::Print()
	{
		memblock_t	*block;

		Con_Printf("zone size: %i  location: %p\n", size, this);

		for (block = blocklist.next; ; block = block->next)
		{
			Con_Printf("block:%p    size:%7i    tag:%3i\n",
				block, block->size, block->tag);

			if (block->next == &blocklist)
				break;			// all blocks have been hit	
			if ((uint8_t *)block + block->size != (uint8_t *)block->next)
				Con_Printf("ERROR: block size does not touch the next block\n");
			if (block->next->prev != block)
				Con_Printf("ERROR: next block doesn't have proper back link\n");
			if (!block->tag && !block->next->tag)
				Con_Printf("ERROR: two consecutive free blocks\n");
		}
	}
	Memory::memzone_t::memzone_t(uint32_t size)
	{
		memblock_t	*block;

		// set the entire zone to one free block

		this->blocklist.next = this->blocklist.prev = block =
			(memblock_t *)((uint8_t *)this + sizeof(memzone_t));
		this->blocklist.tag = 1;	// in use block
		this->blocklist.id = 0;
		this->blocklist.size = 0;
		this->rover = block;

		block->prev = block->next = &this->blocklist;
		block->tag = 0;			// free block
		block->id = ZONEID;
		block->size = size - sizeof(memzone_t);
	}
	Memory::Memory(void* buf, size_t size) {

	}
	void Memory::Free(void* ptr) {
		memblock_t	*block, *other;

		if (!ptr)
			Sys_Error("Z_Free: NULL pointer");

		block = (memblock_t *)((uint8_t *)ptr - sizeof(memblock_t));
		if (block->id != ZONEID)
			Sys_Error("Z_Free: freed a pointer without ZONEID");
		if (block->tag == 0)
			Sys_Error("Z_Free: freed a freed pointer");

		block->tag = 0;		// mark as free

		other = block->prev;
		if (!other->tag)
		{	// merge with previous free block
			other->size += block->size;
			other->next = block->next;
			other->next->prev = other;
			if (block == mainzone->rover)
				mainzone->rover = other;
			block = other;
		}

		other = block->next;
		if (!other->tag)
		{	// merge the next free block onto the end
			block->size += other->size;
			block->next = other->next;
			block->next->prev = block;
			if (other == mainzone->rover)
				mainzone->rover = block;
		}
	}
	void* Memory::Malloc(size_t size) {
		void	*buf;

		CheckHeap();	// DEBUG
		buf = Malloc(size, 1);
		if (!buf)
			Sys_Error("Z_Malloc: failed on allocation of %i bytes", size);
		Q_memset(buf, 0, size);

		return buf;
	}
	void* Memory::Malloc(size_t size, uint32_t tag) {
		int		extra;
		memblock_t	*start, *rover, *newp, *base;

		if (!tag)
			Sys_Error("Z_TagMalloc: tried to use a 0 tag");

		//
		// scan through the block list looking for the first free block
		// of sufficient size
		//
		size += sizeof(memblock_t);	// account for size of block header
		size += 4;					// space for memory trash tester
		size = (size + 7) & ~7;		// align to 8-byte boundary

		base = rover = mainzone->rover;
		start = base->prev;

		do
		{
			if (rover == start)	// scaned all the way around the list
				return NULL;
			if (rover->tag)
				base = rover = rover->next;
			else
				rover = rover->next;
		} while (base->tag || base->size < size);

		//
		// found a block big enough
		//
		extra = base->size - size;
		if (extra >  MINFRAGMENT)
		{	// there will be a free fragment after the allocated block
			newp = (memblock_t *)((uint8_t *)base + size);
			newp->size = extra;
			newp->tag = 0;			// free block
			newp->prev = base;
			newp->id = ZONEID;
			newp->next = base->next;
			newp->next->prev = newp;
			base->next = newp;
			base->size = size;
		}

		base->tag = tag;				// no longer a free block

		mainzone->rover = base->next;	// next allocation will start looking here

		base->id = ZONEID;

		// marker for memory trash testing
		*(int *)((uint8_t *)base + base->size - 4) = ZONEID;

		return (void *)((uint8_t *)base + sizeof(memblock_t));
	}
	void Memory::DumpHeap() {

	}

	void Memory::CheckHeap() {
		memblock_t	*block;

		for (block = mainzone->blocklist.next; ; block = block->next)
		{
			if (block->next == &mainzone->blocklist)
				break;			// all blocks have been hit	
			if ((uint8_t *)block + block->size != (uint8_t *)block->next)
				Sys_Error("Z_CheckHeap: block size does not touch the next block\n");
			if (block->next->prev != block)
				Sys_Error("Z_CheckHeap: next block doesn't have proper back link\n");
			if (!block->tag && !block->next->tag)
				Sys_Error("Z_CheckHeap: two consecutive free blocks\n");
		}
	}
	void Memory::FreeMemory() {

	}

	void *Hunk::Alloc(size_t size) {	// returns 0 filled memory
		return AllocName(size, "unknown");
	}
	
	void *Hunk::AllocName(size_t size,const char *name) {
		hunk_t	*h;

#ifdef PARANOID
		Hunk_Check();
#endif

		if (size < 0)
			Sys_Error("Hunk_Alloc: bad size: %i", size);

		size = sizeof(hunk_t) + ((size + 15)&~15);

		if (hunk_size - hunk_low_used - hunk_high_used < size)
			Sys_Error("Hunk_Alloc: failed on %i bytes", size);

		h = (hunk_t *)(hunk_base + hunk_low_used);
		hunk_low_used += size;

		Cache_FreeLow(hunk_low_used);

		Q_memset(h, 0, size);

		h->size = size;
		h->sentinal = HUNK_SENTINAL;
		Q_strncpy(h->name, name, 8);

		return (void *)(h + 1);
	}

	void *Hunk::HighAllocName(size_t size, const char *name) {
		hunk_t	*h;

		if (size < 0)
			Sys_Error("Hunk_HighAllocName: bad size: %i", size);

		if (hunk_tempactive)
		{
			FreeToHighMark(hunk_tempmark);
			hunk_tempactive = false;
		}

#ifdef PARANOID
		Hunk_Check();
#endif

		size = sizeof(hunk_t) + ((size + 15)&~15);

		if (hunk_size - hunk_low_used - hunk_high_used < size)
		{
			Con_Printf("Hunk_HighAlloc: failed on %i bytes\n", size);
			return NULL;
		}

		hunk_high_used += size;
		Cache_FreeHigh(hunk_high_used);

		h = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);

		memset(h, 0, size);
		h->size = size;
		h->sentinal = HUNK_SENTINAL;
		Q_strncpy(h->name, name, 8);

		return (void *)(h + 1);
	}

	int	Hunk::LowMark() {
		return hunk_low_used;
	}
	void Hunk::FreeToLowMark(int mark) {

		if (mark < 0 || mark > hunk_low_used)
			Sys_Error("Hunk_FreeToLowMark: bad mark %i", mark);
		Q_memset(hunk_base + mark, 0, hunk_low_used - mark);
		hunk_low_used = mark;
	}
	int	Hunk::HighMark() {
		if (hunk_tempactive)
		{
			hunk_tempactive = false;
			FreeToHighMark(hunk_tempmark);
		}
		return hunk_high_used;
	}
	void Hunk::FreeToHighMark(int mark){
		if (hunk_tempactive)
		{
			hunk_tempactive = false;
			FreeToHighMark(hunk_tempmark);
		}
		if (mark < 0 || mark > hunk_high_used)
			Sys_Error("Hunk_FreeToHighMark: bad mark %i", mark);
		Q_memset(hunk_base + hunk_size - hunk_high_used, 0, hunk_high_used - mark);
		hunk_high_used = mark;
	}

	void *Hunk::TempAlloc(size_t size) {
		void	*buf;

		size = (size + 15)&~15;

		if (hunk_tempactive)
		{
			FreeToHighMark(hunk_tempmark);
			hunk_tempactive = false;
		}

		hunk_tempmark = HighMark();

		buf = HighAllocName(size, "temp");

		hunk_tempactive = true;

		return buf;
	}
	void Hunk::Print(bool all) {
		hunk_t	*h, *next, *endlow, *starthigh, *endhigh;
		int		count, sum;
		int		totalblocks;
		char	name[9];

		name[8] = 0;
		count = 0;
		sum = 0;
		totalblocks = 0;

		h = (hunk_t *)hunk_base;
		endlow = (hunk_t *)(hunk_base + hunk_low_used);
		starthigh = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);
		endhigh = (hunk_t *)(hunk_base + hunk_size);

		Con_Printf("          :%8i total hunk size\n", hunk_size);
		Con_Printf("-------------------------\n");

		while (1)
		{
			//
			// skip to the high hunk if done with low hunk
			//
			if (h == endlow)
			{
				Con_Printf("-------------------------\n");
				Con_Printf("          :%8i REMAINING\n", hunk_size - hunk_low_used - hunk_high_used);
				Con_Printf("-------------------------\n");
				h = starthigh;
			}

			//
			// if totally done, break
			//
			if (h == endhigh)
				break;

			//
			// run consistancy checks
			//
			if (h->sentinal != HUNK_SENTINAL)
				Sys_Error("Hunk_Check: trahsed sentinal");
			if (h->size < 16 || h->size + (uint8_t *)h - hunk_base > hunk_size)
				Sys_Error("Hunk_Check: bad size");

			next = (hunk_t *)((uint8_t *)h + h->size);
			count++;
			totalblocks++;
			sum += h->size;

			//
			// print the single block
			//
			memcpy(name, h->name, 8);
			if (all)
				Con_Printf("%8p :%8i %8s\n", h, h->size, name);

			//
			// print the total
			//
			if (next == endlow || next == endhigh ||
				strncmp(h->name, next->name, 8))
			{
				if (!all)
					Con_Printf("          :%8i %8s (TOTAL)\n", sum, name);
				count = 0;
				sum = 0;
			}

			h = next;
		}

		Con_Printf("-------------------------\n");
		Con_Printf("%8i total blocks\n", totalblocks);
	}
	void Hunk::Check() {
		hunk_t	*h;

		for (h = (hunk_t *)hunk_base; (uint8_t *)h != hunk_base + hunk_low_used; )
		{
			if (h->sentinal != HUNK_SENTINAL)
				Sys_Error("Hunk_Check: trahsed sentinal");
			if (h->size < 16 || h->size + (uint8_t *)h - hunk_base > hunk_size)
				Sys_Error("Hunk_Check: bad size");
			h = (hunk_t *)((uint8_t *)h + h->size);
		}
	}




};
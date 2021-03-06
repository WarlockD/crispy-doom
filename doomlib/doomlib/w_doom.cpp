#include "w_doom.hpp"

namespace priv {

}

namespace doom_cpp {

	
	bool WadLoader::loadfile(const char* filename) {
		size_t file_size = 0;
		//FILE* f = fopen(filename, "rb");

		int handle = M_OpenReadOnly(filename);
		if (handle >= 0) {
			size_t file_index = _wad_files.size();
			_wad_files.emplace_back(filename);
			wadinfo_t header;
			//fread(&header, sizeof(wadinfo_t), 1, f);
			M_Read(handle, &header, sizeof(wadinfo_t));
			header.numlumps = util::to_little_edian(header.numlumps);
			header.infotableofs = util::to_little_edian(header.infotableofs);
			// Vanilla Doom doesn't like WADs with more than 4046 lumps
			// https://www.doomworld.com/vb/post/1010985
			if (!strncmp(header.identification, "PWAD", 4) && header.numlumps > 4046)
			{
				//fclose(f);
				M_Close(handle);
				I_Error("Error: Vanilla limit for lumps in a WAD is 4046, "
					"PWAD %s has %d", filename, header.numlumps);
			}
			vector_type<filelump_t> filelumps(size_t(header.numlumps));
			M_Seek(handle, SeekPos::Begin, header.infotableofs);
		//	fseek(f, header.infotableofs, SEEK_SET);
			assert(M_Read(handle, filelumps.data(), sizeof(filelump_t)* header.numlumps) == sizeof(filelump_t)* header.numlumps);
		//	assert(fread(filelumps.data(), sizeof(filelump_t), header.numlumps, f) == sizeof(filelump_t)* header.numlumps);
			_lumps.reserve(_lumps.size() + header.numlumps);
			for (const filelump_t& flump : filelumps) {
				_lumps.emplace_back(file_index, flump.name, util::to_little_edian(flump.filepos), util::to_little_edian(flump.size));
			}
			M_Close(handle);
			//fclose(f);
			return true;
		}
		return false;
	}

	void WadLoader::ReadLump(lumpindex_t lump, void* dest) {
		if (size_t(lump) >= _lumps.size()) I_Error("W_ReadLump: %i >= numlumps", lump);
		const auto &l = _lumps[lump];
		const auto & fn = _wad_files[l.fileindex];
		int handle = fn.handle;
		if(handle == -1)
			M_OpenReadOnly(fn.filename.c_str());
		M_Seek(handle, SeekPos::Begin, l.position);
		size_t read = M_Read(handle, dest, l.size);

		if (read < l.size)
			I_Error("W_ReadLump: only read %i of %i on lump %i", read, l.size, lump);
	}
	//
	size_t WadLoader::LumpLength(lumpindex_t lump)
	{
		if (size_t(lump) >= _lumps.size()) I_Error("W_LumpLength: %i >= numlumps", lump);
		return _lumps[lump].size;
	}
	lumpindex_t  WadLoader::CheckNumForName(const name8& name) {
		for (size_t i = 0; i < _lumps.size(); i++) {
			if (_lumps[i].name == name) return lumpindex_t(i);
		}
		return lumpindex_t(-1);
	}
	lumpindex_t  WadLoader::GetNumForName(const name8& name)
	{
		lumpindex_t i = CheckNumForName(name);
		if (i < 0) I_Error("W_GetNumForName: %s not found!", name);
		return i;
	}
	void *WadLoader::CacheLump(lumpindex_t lump, PU tag) {
		if (size_t(lump) >= _lumps.size()) I_Error("W_CacheLumpNum: %i >= numlumps", lump);
		auto &l = _lumps[lump];
		if (l.cache != nullptr) {
			Z_ChangeTag(l.cache, tag);
			return l.cache;
		}
		else {
			// not loaded
			l.cache = Z_Malloc(l.size, tag, &l.cache);
			ReadLump(lump, l.cache);
			return l.cache;
		}

	}
	void *WadLoader::CacheLump(const name8& name, PU tag)
	{
		return CacheLump(GetNumForName(name), tag);
	}
	void WadLoader::ReleaseLump(lumpindex_t lump) {
		if (size_t(lump) >= _lumps.size()) I_Error("ReleaseLump: %i >= numlumps", lump);
		auto &l = _lumps[lump];
		if(l.cache != nullptr)
			Z_ChangeTag(l.cache, PU::CACHE);
	}
	void WadLoader::ReleaseLump(const name8& name) {
		ReleaseLump(GetNumForName(name));
	}
	void WadLoader::debug_list_lumps() {
		util::fixed_string<10> zero_terminated;
		for (size_t i = 0; i < _lumps.size(); i++) {
			auto& l = _lumps[i];
			printf("%5i: ", i);
			zero_terminated = l.name;
			printf("%9s ", zero_terminated.data());
			printf("%8i %8i\r\n", l.size, l.position);
		}
	}
}
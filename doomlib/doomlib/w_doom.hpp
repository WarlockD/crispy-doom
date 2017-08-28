#pragma once
#include "doom_common.hpp"

namespace doom_cpp {

	/** A container allowing for caching of arrays of CacheableDoomObjects
	*
	*  It's a massive improvement over the older system, allowing for proper
	*  caching and auto-unpacking of arrays of CacheableDoomObjects and much
	*  cleaner code throughout.
	*
	*  The container itself is a CacheableDoomObject....can you feel the
	*  abuse? ;-)
	*
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<CacheableDoomObject, T>::value>>
	class CacheableDoomObjectContainer : public  CacheableDoomObject {
	public:
		using vector_traits = vector_traits<T>;
		using vector_type = typename vector_traits::vector_type;
		using value_type = typename vector_traits::value_type;
		using iterator = typename vector_traits::iterator;
		using const_iterator = typename vector_traits::const_iterator;
		CacheableDoomObjectContainer() = delete;
		CacheableDoomObjectContainer(const vector_type& stuff) : _stuff(stuff) {}
		CacheableDoomObjectContainer(vector_type&& stuff) : _stuff(std::move(stuff)) {}
		vector_type& getStuff() { return _stuff; }
		const vector_type& getStuff() const { return _stuff; }

		void unpack(std::istream& buf) override { for (value_type& a : _stuff) a.unpack(buf); }
		template<typename CONTAINER>
		static void unpack(std::istream&  buf, CONTAINER&& stuff) {
			for (value_type& a : _stuff) a.unpack(buf);
		}
	private:
		vector_type _stuff;
	};
	// eideness stuff to worry about
	

	static uint32_t stringToInt(const char* src, size_t len) {
		uint8_t s[5] = { 0 ,0,0,0 };
		for (size_t i = 0; i < 4 && *src;)
			s[i++] = *src++;

		return (s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3];
	}
	enum class animenum_t {
		ANIM_ALWAYS,
		ANIM_RANDOM,
		ANIM_LEVEL
	};

	enum class  li_namespace_t {
		ns_global,
		ns_sprites,
		ns_flats,
		ns_colormaps,
		ns_prboom,
		ns_demos,
		ns_hires //e6y
	}; // haleyjd 05/21/02: renamed from "namespace"
	enum class wad_source_t {
		// CPhipps - define elements in order of 'how new/unusual'
		source_iwad,    // iwad file load 
		source_pre,       // predefined lump
		source_auto_load, // lump auto-loaded by config file
		source_pwad,      // pwad file load
		source_lmp,       // lmp file load
		source_net        // CPhipps
						  //e6y
						  //  ,source_deh_auto_load
		, source_deh
		, source_err

	};
	struct wadinfo_t {
		char		identification[4];
		int			numlumps;
		int			infotableofs;
	};


	using lumpindex_t = int32_t;
	class WadLoader {

		// On Windows, vsnprintf() is _vsnprintf().
#ifdef _WIN32
#ifdef _MSC_VER /* not needed for Visual Studio 2008 */
#pragma pack(push,1)
		struct filelump_t {
			int	filepos;
			int	size; // Is INT 32-bit in file!
			char		name[8];
		};
		struct lump_info_t {
			name8			name;
			uint32_t		position;
			uint32_t		size;
			void*			cache;
			int				fileindex;
			// Used for hash table lookups
			lumpindex_t next;
			lump_info_t(int fileindex, name8 name, uint32_t position, uint32_t size) : name(name), position(position), size(size), cache(nullptr), fileindex(fileindex), next(-1) {}
			lump_info_t() = default;
		};
		struct file_name_t {
			string_type filename;
			int handle;
			file_name_t(const string_type& filename) : filename(filename), handle(-1) {}
			file_name_t(string_type&& filename) : filename(std::move(filename)), handle(-1) {}
		};
#pragma pack(pop)
#endif
#endif
		using lump_map_t = map_type<std::reference_wrapper<const name8>, const filelump_t*>;
		vector_type<lump_info_t> _lumps;
		vector_type<file_name_t> _wad_files;
	public:
		WadLoader() {}
		lumpindex_t CheckNumForName(const name8& name);
		lumpindex_t GetNumForName(const name8& name);
		//
		// W_LumpLength
		// Returns the buffer size needed to load the given lump.
		//
		size_t LumpLength(lumpindex_t lump);
		bool WadLoader::loadfile(const char* filename);
		void ReadLump(lumpindex_t lump, void* dest);
		void *CacheLump(lumpindex_t lumpnum, PU tag);
		void *CacheLump(const name8& name, PU tag);
		void ReleaseLump(lumpindex_t lump);
		void ReleaseLump(const name8& name);
		void debug_list_lumps();
		//
		// W_GetNumForName
		// Calls W_CheckNumForName, but bombs out if not found.
		//


	};




}
#pragma once
#include "doom_common.hpp"

namespace doom_cpp {
	// WARNING! ONLY WORKS IN LITTLE EADEN RIGHT NOW
	class filelump_t  : public  IReadableDoomObject, IWritableDoomObject {
		struct _filelump_t {
			uint32_t	filepos;
			uint32_t	size; // Is INT 32-bit in file!
			char		name[8];
		};
		static constexpr size_t filelump_size = 4 + 4 + 8;
		static_assert(sizeof(_filelump_t) == filelump_size, "Lump size not correct?");
		_filelump_t _lump;
	public:
		filelump_t() : _lump{ 0,0, "EMPTY00" } {}
		filelump_t(uint32_t filepos_, uint32_t size_, const char* name_) {
			_lump.filepos = filepos_;
			_lump.size = size_;
			std::strncpy(_lump.name, name_, 8);
		}

		void read(std::istream& s) final override {
			util::StreamReader r(s);
			// MAES: Byte Buffers actually make it convenient changing byte order on-the-fly.
			// But RandomAccessFiles (and inputsteams) don't :-S
			r.read(_lump.filepos);
			r.read(_lump.size);
			r.read(_lump.name, 8);
		}
		void write(std::ostream& s) const final override {
			util::StreamWriter w(s);
			// MAES: Byte Buffers actually make it convenient changing byte order on-the-fly.
			// But RandomAccessFiles (and inputsteams) don't :-S
			w.write(_lump.filepos);
			w.write(_lump.size);
			w.write(_lump.name, 8);

		}

		bool compressed() const { return _lump.name[0] > 0x7F; }
		uint32_t filepos() const { return _lump.filepos; }
		uint32_t size() const { return _lump.filepos; }
		string_view name() const { return string_view(_lump.name,8); }

#if 0
			throws IOException {
			if (!big_endian) {
				DoomIO.writeLEInt(dos, (int)filepos);
				DoomIO.writeLEInt(dos, (int)size);
			}
			else {
				dos.writeInt((int)filepos);
				dos.writeInt((int)size);
			}
			DoomIO.writeString(dos, name, 8);

		}
#endif

	};
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
	template<typename T, typename = std::enable_if<std::is_base_of<CacheableDoomObject,T>::value>>
	class CacheableDoomObjectContainer: public  CacheableDoomObject {
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
		static void unpack(std::istream&  buf, CONTAINER&& stuff)  {
			for (value_type& a : _stuff) a.unpack(buf);
		}
	private:
		vector_type _stuff;
	};
	// eideness stuff to worry about
	class name8 : public util::hash_interface {
		union {
			uint64_t number;
			char str[8];
		} _name;
	public:
		name8() { _name.number = 0UL; }
		name8(const char* str_) { std::strncpy(_name.str, str_, 8); }
		uint32_t int_name() const { return _name.number >> 32; }
		size_t hash() const { return sizeof(size_t) == sizeof(uint32_t) ? int_name() : _name.number; }
		bool operator==(const char* r) const { return std::strncmp(_name.str,r , 8) == 0; }
		bool operator==(const name8& r) const { return _name.number == r._name.number; }
		bool operator!=(const name8& r) const { return !(*this == r);   }
		bool operator!=(const char* r) const { return !(*this == r); }

		static uint32_t stringToInt(const char* src, size_t len) {
			uint8_t s[5] = { 0 ,0,0,0 };
			for (size_t i = 0; i < 4 && *src) 
				s[i++] = *src++;

			return (s[0] << 24) | (s[ 1] << 16) | (s[2] << 8) | s[3];
		}
	};

	enum class animenum_t {
		ANIM_ALWAYS,
		ANIM_RANDOM,
		ANIM_LEVEL
	};

	enum class  li_namespace_t  {
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
	// CPhipps - changed wad init
	// We _must_ have the wadfiles[] the same as those actually loaded, so there 
	// is no point having these separate entities. This belongs here.
	class wadinfo_t : public IReadableDoomObject, IWritableDoomObject {
		struct _wadinfo_t {
			union {
				uint32_t id;
				char identification[4];
			};
			uint32_t numlumps;
			uint32_t infotableofs;
		};
		_wadinfo_t _info;
	public:
		uint32_t identification() const { return _info.id; }
		uint32_t numlumps() const { return _info.numlumps; }
		uint32_t infotableofs() const { return _info.infotableofs; }
		void read(std::istream& s) final override {
			util::StreamReader r(s);
			// MAES: Byte Buffers actually make it convenient changing byte order on-the-fly.
			// But RandomAccessFiles (and inputsteams) don't :-S
			r.read(_info.identification,4);
			r.read(_info.numlumps);
			r.read(_info.numlumps);
		}
		void write(std::ostream& s) const final override {
			util::StreamWriter w(s);
			// MAES: Byte Buffers actually make it convenient changing byte order on-the-fly.
			// But RandomAccessFiles (and inputsteams) don't :-S
			w.write(_info.identification, 4);
			w.write(_info.numlumps);
			w.write(_info.numlumps);

		}
	};

	struct wadfile_info_t {
		string_type name; // Also used as a resource identifier, so save with full path and all.
		int type; // as per InputStreamSugar
		wad_source_t src;
		bool cached; // Whether we use local caching e.g. for URL or zips
		long maxsize = -1; // Update when known for sure. Will speed up seeking.
	};

	struct lumpinfo_t
	{
		// WARNING: order of some fields important (see info.c).

		char  name[9];
		int   size;

		// killough 4/17/98: namespace tags, to prevent conflicts between resources
		li_namespace_t li_namespace
		wadfile_info_t *wadfile;
		int position;
		wad_source_t source;
		int flags; //e6y
	} ; */
	// enums
	




}
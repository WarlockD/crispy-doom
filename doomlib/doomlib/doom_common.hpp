#pragma once


#include "ByteBuffer.hpp"
#include <cstdint>
#include <istream>
#include <ostream>
#include <vector>
#include <cassert>
#include <string_view>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace util {
	using string_view = std::string_view;
#define DOOM_ASSERT(C) assert((C))
// sooo... in case we are disabling throws as in embeded stuff, we want to use this
#define DOOM_THROW(V) throw (V)
	constexpr static bool isLittleEndian = true;
	template<size_t HASH_LIMIT=8>
	static inline size_t luaS_hash(const char *str, size_t l, unsigned int seed= 5381) {
		size_t h = seed ^ l;
		size_t step = (l >> HASH_LIMIT) + 1;
		for (; l >= step; l -= step)
			h ^= ((h << 5) + (h >> 2) + static_cast<uint8_t>(str[l - 1]));
		return h;
	}
	struct hash_interface {
		virtual size_t hash() const = 0;
		~hash_interface() {}
	};
	
	template<size_t TYPE_SIZE>
	inline static void edian_byte_swap(char* bits) {
		static_assert((TYPE_SIZE & 1) == 0, "Size is not div by 2?");
		for (size_t k = 0; k < (TYPE_SIZE >> 1); k++)
			std::swap(bits[k], bits[TYPE_SIZE - k - 1]);
	}
	template<>
	inline static void edian_byte_swap<1>(char* bits) { }
	template<>
	inline static void edian_byte_swap<2>(char* bits) {
		std::swap(bits[0], bits[1]);
	} // does nothing
	template<>
	inline static void edian_byte_swap<4>(char* bits) {
		std::swap(bits[0], bits[3]);
		std::swap(bits[1], bits[2]);
	} // does nothing
	template<>
	inline static void edian_byte_swap<8>(char* bits) {
		std::swap(bits[0], bits[7]);
		std::swap(bits[1], bits[6]);
		std::swap(bits[2], bits[5]);
		std::swap(bits[3], bits[4]);
	}
	template<typename T, size_t S = sizeof(T)>
	inline static T edian_swap(T value) { 
		edian_byte_swap<s>(reinterpret_cast<char*>(&value)); 
		return value; 
	}

	template<typename T, size_t S = sizeof(T)>
	inline static T to_little_edian(T value) {
		if (isLittleEndian) edian_byte_swap<S>(reinterpret_cast<char*>(&value));
		return value;
	}
	template<typename T, size_t S = sizeof(T)>
	inline static T to_big_edian(T value) {
		if (!isLittleEndian) edian_byte_swap<S>(reinterpret_cast<char*>(&value));
		return value;
	}


	// constexpr stuff
	template<size_t N>
	struct c_string
	{
		size_t length;
		char str[N + 1];

		constexpr explicit c_string(size_t p_length)
			: length(p_length), str{}
		{}
	};

	template<size_t M>
	constexpr auto make_c_string(char const (&str)[M])
	{
		c_string<M - 1> ret{ M - 1 };
		for (int i = 0; i < M; ++i)
		{
			ret.str[i] = str[i];
		}
		return ret;
	}

	template<size_t N, size_t M>
	constexpr auto join(c_string<N> const& x, c_string<M> const& y)
	{
		c_string<N + M> ret{ x.length + y.length };

		for (int i = 0; i < x.length; ++i)
			ret.str[i] = x.str[i];
		for (int i = 0; i < y.length; ++i)
			ret.str[i + x.length] = y.str[i];

		ret.str[N + M] = '\0';

		return ret;
	}
	template<size_t N, size_t M>
	constexpr auto operator+(c_string<N> const& x, c_string<M> const& y)
	{
		return join(x, y);
	}
	template<typename T>
	constexpr void c_swap(T& x, T& y)
	{
		T tmp(std::move(x));
		x = std::move(y);
		y = std::move(tmp);
	}
	// from http://en.cppreference.com/w/cpp/algorithm/reverse
	template<typename I>
	constexpr void reverse(I beg, I end)
	{
		while (beg != end && beg != --end)
		{
			c_swap(*beg, *end);
			++beg;
		}
	}
	template<typename T>
	constexpr auto c_abs(T x) { return x < T{ 0 } ? -x : x; }


	// https://github.com/fmatthew5876/stdcxx-ascii/blob/master/include/ascii.hh
	constexpr bool isdigit(int c) noexcept {
		return c >= '0' && c <= '9';
	}

	constexpr bool isxdigit(int c) noexcept {
		return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
	}

	constexpr bool islower(int c) noexcept {
		return c >= 'a' && c <= 'z';
	}

	constexpr bool isupper(int c) noexcept {
		return c >= 'A' && c <= 'Z';
	}

	constexpr bool isalpha(int c) noexcept {
		return islower(c) || isupper(c);
	}
	constexpr bool isalnum(int c) noexcept {
		return isalpha(c) || isdigit(c);
	}
	constexpr bool isspace(int c) noexcept {
		return c == ' ' || (c >= '\t' && c <= '\r');
	}
	constexpr int tolower(int c) noexcept {
		return isupper(c) ? c - ('z' - 'Z') : c;
	}

	constexpr int toupper(int c) noexcept {
		return islower(c) ? c + ('z' - 'Z') : c;
	}


	//New: Converts character c into a digit, and then multiples by m
	//Pre-Condition: std::ascii::isdigit(c) == true
	constexpr int todigit(int c) noexcept {
		return (c - '0');
	}
	//New: Converts character c into a digit, and then multiples by m
	//Pre-Condition: std::ascii::isdigit(c) == true
	//Use:
	//  auto ys = "2014";
	//  int y = todigit(ys[0], 1000) + todigit(ys[1], 100) + todigit(ys[2], 10) + todigit(ys[3]);
	//  assert(y == 2014);
	constexpr int todigit(int c, int m) noexcept {
		return todigit(c) * m;
	}


	//New: Converts character c into a hex digit
	//Pre-Condition isdigit(c) == true
	constexpr int toxdigit(int c) noexcept {
		if (c >= 'a' && c <= 'f') {
			return (c - 'a');
		}
		if (c >= 'A' && c <= 'F') {
			return (c - 'A');
		}
		return todigit(c);
	}

	//New: Converts character c into a hex digit and then multiples by m
	//Pre-Condition isdigit(c) == true
	constexpr int toxdigit(int c, int m) noexcept {
		return toxdigit(c) * m;
	}


	static int strcasecmp(const char* l, const char* r, size_t n) {
		if (n == 0) return 0;
		do {
			char cl = tolower(*l++);
			char cr = tolower(*r++);
			if (--n == 0 || cl != cr || cl == '\0' || cr == '\0') return cl - cr;
		} while (1);
	}
	static int strcasecmp(const char* l, const char* r) {
		do {
			char cl = tolower(*l++);
			char cr = tolower(*r++);
			if (cl != cr || cl == '\0' || cr == '\0') return cl - cr;
		} while (1);
	}
	static int strcasecmp(const string_view& l, const string_view& r) {
		auto cli = l.begin();
		auto cri = r.begin();
		do {
			char cl = tolower(*cli++);
			char cr = tolower(*cri++);
			if (cl != cr || cli == l.end() || cri == r.end()) return cl - cr;
		} while (1);
	}
	template<typename T, typename = std::enable_if<std::is_signed<T>::value>>
	constexpr auto ntoa(T n)
	{
		c_string< std::numeric_limits<T>::digits10 + 1 > ret{ 0 };
		int pos = 0;

		T cn = n;
		do
		{
			ret.str[pos] = '0' + c_abs(cn % 10);
			++pos;
			cn /= 10;
		} while (cn != T{ 0 });

		if (n < T{ 0 })
		{
			ret.str[pos] = '-';
			++pos;
		}

		ret.str[pos] = '\0';
		ret.length = pos;

		reverse(ret.str, ret.str + ret.length);
		return ret;
	}

	// not supported by the libstdc++ at coliru
	//template<class T, class = std::enable_if_t< std::is_arithmetic<T>{} >>
	template<class T, class = typename std::enable_if<std::is_arithmetic<T>{}>::type>
	constexpr auto to_c_string(T p)
	{
		return ntoa(p);
	}
	template<int N>
	constexpr auto to_c_string(char const (&str)[N])
	{
		return make_c_string(str);
	}

	template<class T, class U, class... TT>
	constexpr auto to_c_string(T&& p0, U&& p1, TT&&... params)
	{
		return   to_c_string(std::forward<T>(p0))
			+ to_c_string(std::forward<U>(p1), std::forward<TT>(params)...);
	}
	class path_helper {
		string_view _fullpath;
		string_view _extension;
		string_view _filename;
		string_view _path;
	public:
		inline static constexpr bool is_dir_char(int v) { return v == '/' || v == '\\'; }
		static string_view get_filename(const string_view& sv) {
			if (sv.size() != 0) {
				for (int pos = sv.size() - 1; pos >= 0; pos--) {
					if (is_dir_char(sv.at(pos))) return sv.substr(pos + 1);
				}
			}
			return string_view();
		}
		static string_view get_extension(const string_view& sv) {
			if (sv.size() != 0) {
				for (int pos = sv.size() - 1; pos >= 0; pos--) {
					if (sv.at(pos) == '.') return sv.substr(pos + 1);
				}
			}
			return string_view();
		}
		path_helper(string_view path) : _fullpath(path), _extension(), _filename(), _path() {
			if (_fullpath.size() != 0) {
				bool found_extension = false;
				int dot_pos = -1;
				int pos = 0;
				for (; pos >= 0; pos--) {
					char c = _path.at(pos);
					if (is_dir_char(c)) break; // end of file name
					if (dot_pos == -1 && c == '.') dot_pos = pos;
				}
				if (dot_pos != -1) _extension = _fullpath.substr(dot_pos);
				if (pos > 0) _filename = _fullpath.substr(pos + 1);
				if (pos != 0) _path = _fullpath.substr(0, pos);
			}
		}
		path_helper(const char* path) : path_helper(string_view(path)) {}
		const string_view& path() const { return _path; }
		const string_view& extension() const { return _path; }
		const string_view& filename() const { return _path; }
		bool match_extension(const char* path) const {
			return strcasecmp(_extension, get_extension(path));
		}
		bool match_filename(const char* filename) const {
			return strcasecmp(_filename, get_extension(filename));
		}
	};
};
namespace std {
	template<>
	struct hash<util::hash_interface> {
		size_t operator()(const util::hash_interface& r) const { return r.hash(); }
	};
}
namespace doom_cpp {
	// if we are a fixed string, then it fills the 8 slot, otherwise we are a 
	// refrence to a string
	// The only point of this 

//	using string_type = util::cstring;
	using string_view = std::string_view;
	template<typename T>
	struct vector_traits {
		using value_type = T;
		using vector_type = std::vector<value_type>;
		using iterator = typename vector_type::iterator;
		using const_iterator = typename vector_type::const_iterator;
	};

	class fixed_t : public util::hash_interface {
		int32_t _val;
	public:
		static constexpr size_t FRACBITS = 16;
		static constexpr size_t FRACUNIT = (1 << FRACBITS);;
	//	static constexpr size_t MAPFRACUNIT = FRACUNIT / Defines.TIC_MUL;
		constexpr static inline uint32_t FixedMul(uint32_t l, uint32_t r) {
			return static_cast<uint32_t>((static_cast<uint64_t>(l) * static_cast<uint64_t>(r)) >> FRACBITS);
		}
		constexpr static inline int32_t FixedMulInt(int32_t l, int32_t r) {
			return  static_cast<uint32_t>((static_cast<int64_t>(l) * static_cast<int64_t>(r)) >> FRACBITS);
		}
		template<typename T>
		constexpr static inline T abs(T v) { return v < 0 ? -v : v; }
		constexpr static inline int32_t FixedDiv2(int32_t l, int32_t r) {
			return static_cast<int64_t>((static_cast<int64_t>(l) << 16) / r);
		}
		constexpr static inline int32_t FixedDiv(int32_t l, int32_t r) {
			return ((abs(l) >> 14) >= abs(r)) ?
				((l*r) < 0) ? std::numeric_limits<int32_t>::min() : std::numeric_limits<int32_t>::max()
				: FixedDiv2(l, r);
		}

		fixed_t() : _val(0) {}
		fixed_t(int32_t val) : _val(0) {}
		operator int32_t() const { return _val; }
		operator bool() const { return _val != 0; }
		fixed_t operator*(const int32_t&r) const {return fixed_t(FixedMul(_val, r));}
		fixed_t operator*(const fixed_t&r) const { return fixed_t(FixedMul(_val, r._val)); }
		fixed_t& operator*=(const fixed_t&r) { return 	(*this = *this * r); }
		fixed_t& operator*=(const int32_t&r) { return 	(*this = *this * r); }
		fixed_t operator+(const int32_t&r) const { return fixed_t(_val+ r); }
		fixed_t operator+(const fixed_t&r) const { return fixed_t(_val+ r._val); }
		fixed_t& operator+=(const fixed_t&r) { return 	(*this = *this + r); }
		fixed_t& operator+=(const int32_t&r) { return 	(*this = *this + r); }
		fixed_t operator-(const int32_t&r) const { return fixed_t(_val + r); }
		fixed_t operator-(const fixed_t&r) const { return fixed_t(_val + r._val); }
		fixed_t& operator-=(const fixed_t&r) { return 	(*this = *this + r); }
		fixed_t& operator-=(const int32_t&r) { return 	(*this = *this + r); }
		size_t hash() const { return _val; }
		bool operator==(const fixed_t&r) const { return _val == r._val; }
		bool operator!=(const fixed_t&r) const { return _val != r._val; }
		bool operator>=(const fixed_t&r) const { return _val >= r._val; }
		bool operator<=(const fixed_t&r) const { return _val <= r._val; }
		bool operator<(const fixed_t&r) const { return _val < r._val; }
		bool operator>(const fixed_t&r) const { return _val > r._val; }

	};
	// so Istream manages itself.  once a istream is created, it can self open or close on its own.
	// so when you do a read, it might just open the file at the last offset, and then close it
	// or keep the handle open, etc.
	struct IStreamBase {
		virtual string_view name() const = 0; // name of stream, or where the file location is, etc
		virtual size_t size() const = 0; // size of stream
		virtual size_t offset() const = 0; // offset from start of file
		virtual void offset(size_t i) = 0; // set offset of steream from start
		virtual bool readonly() const = 0;
		virtual bool open() = 0;
		virtual void close() = 0;
		virtual ~IStreamBase() {}
	};

	struct IStreamReader : public IStreamBase {
		virtual ~IStreamReader() {}
		virtual size_t read(char* data, size_t count) = 0;
		template<typename T>
		size_t read(T* value, size_t size) {
			char* bits = reinterpret_cast<char*>(value);
			size *= sizeof(T);
			size_t readbytes = read(bits, size);
			assert(readbytes == size);
			return size;
		}
		template<typename T>
		size_t	read(T& value) {
			return read(reinterpret_cast<char*>(&value), sizeof(T));
		}
		template<typename T, bool SWAP_ENDIAN = false, typename = std::enable_if<std::is_arithmetic<T>::value>>
		size_t	readvalue(T& value) {
			size_t size = read(reinterpret_cast<char*>(value), sizeof(T));
			if(size == sizeof(T) && SWAP_ENDIAN)  priv::edian_swap<type_size>(bits);
			return size;
		}
	};

	struct IStreamWriter : public IStreamBase {
		virtual ~IStreamWriter() {}
		virtual size_t write(const char* data, size_t count) = 0;
		template<typename T>
		size_t write(T* value, size_t size) {
			char* bits = reinterpret_cast<char*>(value);
			size *= sizeof(T);
			size_t written = write(bits, size);
			assert(written == size);
			return size;
		}
		template<typename T>
		size_t	write(T& value) {
			return write(reinterpret_cast<const char*>(&value), sizeof(T));
		}
		template<typename T, bool SWAP_ENDIAN = false, typename = std::enable_if<std::is_arithmetic<T>::value>>
		size_t	writealue(T& value) {
			constexpr size_t type_size = sizeof(T);
			size_t written = 0;
			if (SWAP_ENDIAN) {
				union {
					T value;
					char bits[type_size];
				} uvalue;
				uvalue.value = value;
				priv::edian_swap<type_size>(uvalue.bits);
				written = write(uvalue.bits, type_size);
			}
			else {
				const char* bits = reinterpret_cast<const char*>(&value);
				written = write(bits, type_size);
			}
			assert(written == type_size);
			// all modern compilers unroll loops so this should be small
			// else we will have to use intritics

			return type_size;
		}
	};
	struct FileStream : public IStreamWriter, public IStreamReader { };

	FileStream* I_LoadFileStream(const char* path);



	struct IReadableDoomObject {
		virtual void read(IStreamReader&) = 0;
		~IReadableDoomObject() {}
	};
	struct IWritableDoomObject {
		virtual void write(IStreamWriter&) const = 0;
		~IWritableDoomObject() {}
	};
	/// This is for objects that can be read from disk, but cannot
	/// self-determine their own length for some reason.
	struct AidedReadableDoomObject {
		virtual void read(std::istream& f, size_t len) = 0;
		~AidedReadableDoomObject() {}
	};
	/** All objects that can be deserialized from raw byte buffers such as those
	*  read from WAD lumps should implement this method, so that the WadLoader
	*  can cache them, and recursive calls to  sub-objects can be made.
	*
	*  E.g. an object of type A consists of a header and a list of objects of type B.
	*  Calling A.unpack(buf) will cause A to unmarshal its own header, set the list of
	*  B objects, and then call B.unpack() for each of them, by passing the same buffer
	*  along.
	*
	*  This system works cleanly, and allows to simulate Doom's "cached memory" while
	*  returning proper objects of the correct type and keeping close to Java's
	*  "correct" way of doing things.
	*   *
	*  For example, if a patch_t is read from disk, the WadLoader uses its unpack() method
	*  to read it from a lump read from disk, and creates a new patch_t object, which is placed
	*  in the lump cache (which holds CacheableDoomObject, incidentally). The next time this
	*  same patch_t is requested, the reference to the already cached patch_t will be returned,
	*  if it hasn't been forcedly flushed from the cache. Voila', lump caching!
	*
	*  The principle can be applied to ARRAYS of similar objects too: using the same buffer,
	*  iterative serial unpacking is possible, while still mantaining a "cached" reference
	*  to their array (TODO: actually, this needs to be implemented more efficiently. Look in
	*  WadLoader)
	*
	*  The opposite would be a "PackableDoomObject", aka objects that can pack themselves into
	*  a byte buffer for transmission purposes, although Doom doesn't really need to write as
	*  much as it needs reading.
	*
	*  For the purpose of saving/loading games, which need to read/write to variable disk
	*  structures ALL the time, use the ReadableDoomObject/WritableDoomObject interfaces.
	*  Their difference is that they are highly mutable and supposed to be read from files
	*  or input/output streams, and that a continuous reference to them as deserialized
	*  objects (e.g. in the caching mechanism) is not needed.
	*
	*
	* @author Velktron
	*
	*/
	struct CacheableDoomObject {
		virtual void unpack(std::istream& buf) = 0;
		~CacheableDoomObject() {}
	};
	struct IPackableDoomObject {
		virtual void pack(std::ostream& buf) const = 0;
		~IPackableDoomObject() {}
	};

	//
	// M_CheckParm
	// Checks for the given parameter
	// in the program's command line arguments.
	// Returns the argument number (1 to argc-1)
	// or 0 if not present
	//
	int M_OpenReadOnly(const char* path);
	enum class SeekPos {
		Begin,
		Current,
		End
	};
	int M_Seek(int handle, SeekPos origin, std::ptrdiff_t offset);
	int M_Read(int handle, void* buffer, size_t len);
	size_t M_FileSize(int handle);
	void M_Close(int handle);

	void M_SeteParms(size_t argc, const char** argv);
	size_t M_CheckParmWithArgs(const char *check, size_t num_args);
	static size_t M_CheckParm(const char *check) {return M_CheckParmWithArgs(check, 0); }
	static bool M_ParmExists(const char *check) { return M_CheckParm(check) != 0;}

	extern void I_Error(const char* fmt, ...); // error interface

// might end up moving all of memory allocation here
	// Z memory since its soo important
	enum class  PU : uint32_t
	{
		BAD = 0,
		STATIC = 1,                  // static entire execution time
		SOUND,                       // static while playing
		MUSIC,                       // static while playing
		FREE,                        // a free block
		LEVEL,                       // static until level exited
		LEVSPEC,                     // a special thinker in a level

									 // Tags >= PU_PURGELEVEL are purgable whenever needed.

		PURGELEVEL,
		CACHE,

									 // Total number of different tag types

									 NUM_TAGS

	};

	const char* Z_TagToString(PU tag);
	void	Z_Init(void* memory_base, size_t size);
	void*	Z_Malloc(size_t size, PU tag, void **ptr);

	template<typename T>
	inline void* Z_Malloc(size_t size, PU tag, T** ptr) { return Z_Malloc(size, tag, (void**)ptr);}
	void    Z_Free(void *ptr);
	void    Z_FreeTags(PU lowtag, PU hightag);
	void    Z_DumpHeap(PU lowtag, PU hightag);
	void    Z_FileDumpHeap(FILE *f);
	void    Z_CheckHeap();
	void    Z_ChangeTag2(void *ptr, PU tag, const char *file, size_t line);
	void    Z_ChangeUser(void *ptr, void **user);
	size_t     Z_FreeMemory();
	size_t  Z_ZoneSize();
	size_t Z_UsedMemory();
	//
	// This is used to get the local FILE:LINE info from CPP
	// prior to really call the function in question.
	//
	static inline void Z_ChangeTag(void *ptr, PU tag) { Z_ChangeTag2(ptr, tag, __FILE__, __LINE__); }


	template <typename T, PU TAG = PU::STATIC>
	class doom_allocator
	{
	public:
		typedef T value_type;
		typedef size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		static constexpr PU tag = TAG;
		// \brief I don't know what rebind is for.
		template <typename U>
		struct rebind
		{
			typedef doom_allocator<U, tag> other;
		};


		doom_allocator() {}
		doom_allocator(const doom_allocator<T, TAG>&) noexcept {}

		template<typename AT>
		doom_allocator(const doom_allocator<AT, TAG>&) noexcept {}

		template<typename AT>
		doom_allocator<T, TAG>& operator=(const doom_allocator<AT, TAG>&){	return (*this);}

		const_pointer address(const_reference v) const { return std::addressof(v); }
		pointer address(reference r) const { return std::addressof(v); }
		size_type max_size() const { return static_cast<size_type>(Z_UsedMemory()); }
		pointer allocate(size_type n) const { return static_cast<pointer>(Z_Malloc(n, tag, nullptr)); }
		void deallocate(pointer p,size_type n) const { (void)n; Z_Free(p); }
		pointer allocate(size_type _Count, const void *)
		{	// allocate array of _Count elements, ignore hint
			return (allocate(_Count));
		}

		template<class _Objty,
			class... _Types>
			void construct(_Objty *_Ptr, _Types&&... _Args)
		{	// construct _Objty(_Types...) at _Ptr
			::new ((void *)_Ptr) _Objty(std::forward<_Types>(_Args)...);
		}

		template<class _Uty>
		void destroy(_Uty *_Ptr)
		{	// destroy object at _Ptr
			_Ptr->~_Uty();
		}
	};
	template<class _Ty,
		class _Other> inline
		bool operator==(const doom_allocator<_Ty>&,
			const doom_allocator<_Other>&) noexcept
	{	// test for allocator equality
		return (true);
	}

	template<class _Ty,
		class _Other> inline
		bool operator!=(const doom_allocator<_Ty>&,
			const doom_allocator<_Other>&) noexcept
	{	// test for allocator inequality
		return (false);
	}


	// this is handled in z_doom
	struct doom_object {
		void * operator new (size_t size);
		void * operator new (size_t size, PU tag);
		void * operator new (size_t size, PU tag, void** user);
		void operator delete (void * mem);
	};
	using string_type = std::basic_string<char, std::char_traits<char>, doom_allocator<char>>;

	template<typename T>
	using vector_type = std::vector<T, doom_allocator<T>>;
	template<typename K, typename V, typename HASHER= std::hash<K>, typename EQUAL_TO= std::equal_to<K>>
	using map_type = std::unordered_map <K,V, HASHER, EQUAL_TO, doom_allocator<std::pair<const K, V>>>;


};
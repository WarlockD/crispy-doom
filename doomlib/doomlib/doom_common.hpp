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

namespace util {
#define DOOM_ASSERT(C) assert((C))
// sooo... in case we are disabling throws as in embeded stuff, we want to use this
#define DOOM_THROW(V) throw (V)
	constexpr static bool isLittleEndian = true;
	template<size_t HASH_LIMIT=32>
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
	struct cstring : public hash_interface {
		struct _string {
			size_t len;
			size_t hash;
		//	const char* str;
			//	size_t ref;  // might be better to use shared pointer?
			const char* str() const { return (const char*)(this + 1); }
		};
		std::shared_ptr<_string> _str;
	public:
		cstring() = default;
		cstring(const cstring& copy) = default;
		cstring(cstring&& move) = default;
		cstring(const char* str, size_t len) {
			char* _ptr = new char[len + sizeof(_string) + 1];
			std::strncpy(_ptr + sizeof(_string), str, len);
			_str.reset((_string*)_ptr);
			_str->len = len;
			_str->hash = luaS_hash(str, len);
		}
		size_t size() const { return _str ? _str->len : 0; }
		const char* c_str() const { return _str ? _str->str() : nullptr; }
		bool equals(const char* str, size_t len) const {
			return _str && len == _str->len && std::strncmp(str, _str->str(), len);
		}
		bool equals(const char* str) const { return equals(str, strlen(str)); }
		size_t hash() const final override { return _str ? _str->hash : 0; }
		bool operator==(const cstring& other) const {
			return _str.get() == other._str.get() || equals(other.c_str(), other.size());
		}
		bool operator!=(const cstring& other) const { return !(*this == other); }
	};
	namespace priv {
		template<size_t TYPE_SIZE>
		inline static void edian_swap(char* bits) {
			static_assert((TYPE_SIZE & 1) == 0, "Size is not div by 2?");
			for (size_t k = 0; k < (TYPE_SIZE >> 1); k++)
				std::swap(bits[k], bits[TYPE_SIZE - k - 1]);
		}
		template<>
		inline static void edian_swap<1>(char* bits) { }
		template<>
		inline static void edian_swap<2>(char* bits) {
			std::swap(bits[0], bits[1]);
		} // does nothing
		template<>
		inline static void edian_swap<4>(char* bits) {
			std::swap(bits[0], bits[3]);
			std::swap(bits[1], bits[2]);
		} // does nothing
		template<>
		inline static void edian_swap<8>(char* bits) {
			std::swap(bits[0], bits[7]);
			std::swap(bits[1], bits[6]);
			std::swap(bits[2], bits[5]);
			std::swap(bits[3], bits[4]);
		}
	}
	class StreamWriter {
		std::ostream& _stream;
	public:
		StreamWriter(std::ostream& stream) : _stream(stream) {}
		std::ostream& stream() { return _stream; }
		template<typename T, bool SWAP_ENDIAN = false, typename = std::enable_if<std::is_arithmetic<T>::value>>
		size_t	write(const T& value) const {
			constexpr size_t type_size = sizeof(T);
			if (SWAP_ENDIAN) {
				union {
					T value;
					char bits[type_size];
				} uvalue;
				uvalue.value = value;
				priv::edian_swap<type_size>(uvalue.bits);
				_stream.write(uvalue.bits, type_size);
			}
			else {
				const char* bits = reinterpret_cast<const char*>(&value);
				_stream.write(bits, type_size);
			}
			assert(_stream.bad());
			// all modern compilers unroll loops so this should be small
			// else we will have to use intritics

			return type_size;
		}
		template<typename T>
		size_t write(const T* value, size_t size) const {
			const char* bits = reinterpret_cast<const char*>(value);
			size *= sizeof(T);
			_stream.write(bits, size);
			assert(_stream.bad());
			return size;
		}
	};

	class StreamReader {
		std::istream& _stream;
	public:
		StreamReader(std::istream& stream) : _stream(stream) {}
		const std::istream& stream() const { return _stream; }
		std::istream& stream() { return _stream; }
		template<typename T, bool SWAP_ENDIAN=false, typename = std::enable_if<std::is_arithmetic<T>::value>>
		size_t	read(T& value) {
			constexpr size_t type_size = sizeof(T);
			char* bits = reinterpret_cast<char*>(&value);
			_stream.read(bits, type_size);
			assert(_stream.bad());
			// all modern compilers unroll loops so this should be small
			// else we will have to use intritics
			if (SWAP_ENDIAN) priv::edian_swap<type_size>(bits);
			return type_size;
		}
		template<typename T>
		size_t read(T* value, size_t size) {
			char* bits = reinterpret_cast<char*>(value);
			size *= sizeof(T);
			_stream.read(bits, size);
			assert(_stream.bad());
			return size;
		}
		template<size_t SIZE = 8>
		cstring readNullTerminatedString() {
			static_assert(MAX_SIZE > 0, "Buffer cannot be 0");
			char buffer[SIZE + 1];
			_stream.read(buffer, SIZE);
			buffer[SIZE] = 0;
			return cstring(buffer);
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
	template<typename T>
	using vector_type = std::vector<T>;
	using string_type = util::cstring;
	using string_view = std::string_view;
	template<typename T>
	struct vector_traits {
		using value_type = T;
		using vector_type = std::vector<value_type>;
		using iterator = typename vector_type::iterator;
		using const_iterator = typename vector_type::const_iterator;
	};
	class fixed_t {
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
		constexpr static inline int32_t FixedDiv(int32_t l, int32_t r) {
			return (abs(l) >> 14) >= abs(r)) ? (l*r) < 0
				? std::numeric_limits<int32_t>::min() : std::numeric_limits<int32_t>::max()
				: return static_cast<int64_t>((static_cast<int64_t>(l) << 16) / r);
		}
		constexpr static inline int32_t FixedDiv2(int32_t l, int32_t r) {
			return static_cast<int64_t>((static_cast<int64_t>(l) << 16) / r);
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

		bool operator==(const fixed_t&r) const { return _val == r._val; }
		bool operator!=(const fixed_t&r) const { return _val != r._val; }
		bool operator>=(const fixed_t&r) const { return _val >= r._val; }
		bool operator<=(const fixed_t&r) const { return _val <= r._val; }
		bool operator<(const fixed_t&r) const { return _val < r._val; }
		bool operator>(const fixed_t&r) const { return _val > r._val; }

	};

	struct IReadableDoomObject {
		virtual void read(std::istream&) = 0;
		~IReadableDoomObject() {}
	};
	struct IWritableDoomObject {
		virtual void write(std::ostream&) const = 0;
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

};
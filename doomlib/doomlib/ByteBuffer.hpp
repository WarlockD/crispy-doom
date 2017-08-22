/**
 ByteBuffer
 ByteBuffer.h
 Copyright 2011 - 2013 Ramsey Kant

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Modfied 2015 by Ashley Davis (SgtCoDFish)
 */

#ifndef _ByteBuffer_H_
#define _ByteBuffer_H_

// Default number of uint8_ts to allocate in the backing buffer if no size is provided
#define BB_DEFAULT_SIZE 4096

// If defined, utility functions within the class are enabled
#define BB_UTILITY

// If defined, places the class into a namespace called bb
//#define BB_USE_NS

#include <cstdlib>
#include <cstdint>
#include <cstring>

#include <vector>
#include <memory>
#include <algorithm>


#ifdef BB_UTILITY
#include <iostream>
#include <cstdio>
#include <string>
#endif

#ifdef BB_USE_NS
namespace bb {
#endif
#if 0

class ByteBuffer {
public:
	struct Unpacker {
		virtual bool unpack(const ByteBuffer& buffer) = 0;
		~Unpacker() {}
	};
	ByteBuffer(uint32_t size = BB_DEFAULT_SIZE) { buf.reserve(size); clear(); }
	ByteBuffer(const uint8_t* arr, uint32_t size) { buf.reserve(size); buf.assign(arr, arr + size); }
	ByteBuffer(const ByteBuffer& copy) = default;
	ByteBuffer(ByteBuffer&& move) = default;
	ByteBuffer& operator=(const ByteBuffer& copy) = default;
	ByteBuffer& operator=(ByteBuffer&& move) = default;
	~ByteBuffer() = default;
	// Number of uint8_ts from the current read position till the end of the buffer
	size_t bytesRemaining() const { return size() - rpos; }
	void clear() { wpos = rpos = 0; buf.clear(); }// Clear our the vector and reset read and write positions
	size_t size() const { return buf.size(); }
	auto begin() { return buf.begin(); }
	auto end() { return buf.end(); }
	auto begin() const { return buf.begin(); }
	auto end() const { return buf.end(); }
	//ByteBuffer compact(); // TODO?
	bool equals(const ByteBuffer& other) const { return buf == other.buf; }
	void resize(uint32_t newSize) { buf.resize(newSize); wpos = rpos = 0; }


	// Basic Searching (Linear)
	template<typename T> int32_t find(T key, uint32_t start = 0) {
		int32_t ret = -1;
		uint32_t len = buf.size();
		for (uint32_t i = start; i < len; i++) {
			T data = read<T>(i);
			// Wasn't actually found, bounds of buffer were exceeded
			if ((key != 0) && (data == 0))
				break;

			// Key was found in array
			if (data == key) {
				ret = (int32_t)i;
				break;
			}
		}
		return ret;
	}

	// Replacement
	void replace(uint8_t key, uint8_t rep, uint32_t start, bool firstOccuranceOnly = false) {
		uint32_t len = buf.size();
		for (uint32_t i = start; i < len; i++) {
			uint8_t data = read<uint8_t>(i);
			// Wasn't actually found, bounds of buffer were exceeded
			if ((key != 0) && (data == 0))
				break;

			// Key was found in array, perform replacement
			if (data == key) {
				buf[i] = rep;
				if (firstOccuranceOnly)
					return;
			}
		}
	}
	// Read

	int get(size_t index) const {
		if (index >= size()) return -1;
		else return buf[index];
	}
	int get() const {
		if ((rpos+1) >= size()) return -1;
		else return buf[rpos++];
	}
	int peek() const {
		return ((rpos + 1) >= size())? -1 :  get(rpos);
	}
	bool read(uint8_t& value) const { return _read(value); }
	bool read(uint16_t& value) const { return _read(value); }
	bool read(uint32_t& value) const { return _read(value); }
	bool read(uint64_t& value) const { return _read(value); }
	bool read(int8_t& value) const { return _read(value); }
	bool read(int16_t& value) const { return _read(value); }
	bool read(int32_t& value) const { return _read(value); }
	bool read(int64_t& value) const { return _read(value); }
	bool read(float& value) const { return _read(value); }
	bool read(double& value) const { return _read(value); }

	template<typename T>
	typename std::is_base_of<Unpacker,T>::value,bool>
		read(T& value) const { return value.unpack(*this);  }

	bool write(const uint8_t& value)  { return _append(value); }
	bool write(const uint16_t& value)  { return _append(value); }
	bool write(const uint32_t& value)  { return _append(value); }
	bool write(const uint64_t& value)  { return _append(value); }
	bool write(const int8_t& value)  { return _append(value); }
	bool write(const int16_t& value)  { return _append(value); }
	bool write(const int32_t& value)  { return _append(value); }
	bool write(const int64_t& value)  { return _append(value); }
	bool write(float& value)  { return _append(value); }
	bool write(double& value)  { return _append(value); }



	template<typename T=uint8_t, typename = std::enable_if<!std::is_pointer<T>::value && std::is_arithmetic<std::decay_t<T>::value>>
	const T& at(size_t index) const {
		return *((T*)&buf[index]);
	}
	template<typename T = uint8_t, typename = std::enable_if<!std::is_pointer<T>::value && std::is_arithmetic<std::decay_t<T>::value>>
	T& at(size_t index) {
		return *((T*)&buf[index]);
	}
	typename<typename T>
	typename std::enable_if<std::is_base_of<Unpacker, T>::value, size_t>
	 read(T* list, size_t count,size_t index) const {
		size_t i = 0;
		size_t save = rpos;
		rpos = index;
		for (i = 0; i < count; i++)
			if (!list->unpack(*this)) break;
		rpos = save;
		return i;
	}
	typename<typename T>
		typename std::enable_if<std::is_base_of<Unpacker, T>::value, size_t>
		read(T* list, size_t count) const {
		for (size_t i = 0; i < count; i++)
			if (!list->unpack(*this)) return i;
		return count;
	}
	typename<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value, size_t>  read(T* list, size_t count, size_t index) const {
		count = std::min(count / sizeof(T), (size() - index) / sizeof(T));
		std::memcopy(list, &buf[index], count * sizeof(T));
		return count;
	}
	typename<typename T>
	typename std::enable_if<std::is_arithmetic<T>::value, size_t> read(T* list, size_t count) const {
		count = _read(list, count, rpos);
		rpos += count;
		return count;
	}
#if 0

	template<typename T, typename = std::enable_if<!std::is_pointer<T>::value && std::is_arithmetic<std::decay_t<T>::value>>
	bool read(T& value) const {
		if (read_at(value, rpos)) {
			rpos += sizeof(T); 
			return true;
		}
		return false;
	}
	template<typename T, typename = std::enable_if<!std::is_pointer<T>::value && std::is_arithmetic<std::decay_t<T>::value>>
		bool write_at(const T& value, uint32_t index) const {
			if (index + sizeof(T) <= buf.size()) {
				value = *((T*)&buf[index]);
				return true;
			}
			return false;
		}
#endif
	// Write Functions

	// Buffer Position Accessors & Mutators

	void setReadPos(uint32_t r) {
		rpos = r;
	}

	uint32_t getReadPos() const {
		return rpos;
	}

	void setWritePos(uint32_t w) {
		wpos = w;
	}

	uint32_t getWritePos() const {
		return wpos;
	}

	// Utility Functions
#ifdef BB_UTILITY
private:
	std::string _name;
public:
	void setName(const std::string& n) { _name = n; }
	const std::string& getName() const { return _name; }

	void printInfo() const {
		uint32_t length = buf.size();
		std::cout << "ByteBuffer " << getName().c_str() << " Length: " << length << ". Info Print" << std::endl;
	}

	void printAH()const {
		uint32_t length = buf.size();
		std::cout << "ByteBuffer " << getName().c_str() << " Length: " << length << ". ASCII & Hex Print" << std::endl;

		for (uint32_t i = 0; i < length; i++) {
			std::printf("0x%02x ", buf[i]);
		}

		std::printf("\n");
		for (uint32_t i = 0; i < length; i++) {
			std::printf("%c ", buf[i]);
		}

		std::printf("\n");
	}

	void printAscii() const {
		uint32_t length = buf.size();
		std::cout << "ByteBuffer " << getName().c_str() << " Length: " << length << ". ASCII Print" << std::endl;

		for (uint32_t i = 0; i < length; i++) {
			std::printf("%c ", buf[i]);
		}

		std::printf("\n");
	}

	void printHex() const {
		uint32_t length = buf.size();
		std::cout << "ByteBuffer " << getName().c_str() << " Length: " << length << ". Hex Print" << std::endl;

		for (uint32_t i = 0; i < length; i++) {
			std::printf("0x%02x ", buf[i]);
		}

		std::printf("\n");
	}

	void printPosition() const {
		uint32_t length = buf.size();
		std::cout << "ByteBuffer " << getName().c_str() << " Length: " << length << " Read Pos: " << rpos << ". Write Pos: "
			<< wpos << std::endl;
	}
#endif



private:
	size_t wpos;
	mutable size_t rpos;
	std::vector<uint8_t> buf;

	template<typename T> size_t _read(T* value, size_t len, size_t index) const {
		size_t total_bytes = len + sizeof
		if (index + sizeof(T) > buf.size()) return false;
		value = *((T*)&buf[index]);
		return true;
	}

	template<typename T> bool _read(T& value, size_t index) const {
		if (index + sizeof(T) > buf.size()) return false;
		value = *((T*)&buf[index]);
		return true;
	}
	template<typename T> bool _read(T& value) const {
		if (!_read(value, rpos)) return false;
		rpos += sizeof(T);
		return true;
	}

	template<typename T> bool _append(const T& data) {
		constexpr size_t s = sizeof(T);

		if (size() < (wpos + s))
			buf.resize(wpos + s);
		std::memcpy(&buf[wpos], (const uint8_t*)&data, s);
		//printf("writing %c to %i\n", (uint8_t)data, wpos);

		wpos += s;
		return true;
	}

	template<typename T> bool _insert(const T& data, uint32_t index) {
		if ((index + sizeof(T)) > size())
			return false;

		std::memcpy(&buf[index], (const uint8_t*)&data, sizeof(data));
		wpos = index + sizeof(data);
		return true;
	}
};

#endif
#ifdef BB_USE_NS
}
#endif

#endif

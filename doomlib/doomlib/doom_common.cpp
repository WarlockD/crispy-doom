#include "doom_common.hpp"

// posix interface
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <io.h>  
#include "z_doom.hpp"

namespace doom_cpp {
	static size_t myargc = 0;
	static const char** myargv = nullptr;

	void M_SeteParms(size_t argc, const char** argv) {
		if (argc < 1 || argv == nullptr) {
			myargc = 0; myargv = nullptr;
		}
		else {
			myargc = argc;
			myargv = argv;
		}

	}
	size_t M_CheckParmWithArgs(const char *check, size_t num_args)
	 {
		if (myargc > 1 && myargv != nullptr) {
			for (size_t i = 1; i < myargc - num_args; i++) {
				if (!util::strcasecmp(check, myargv[i])) return i;

			}
		}
		return 0U;
	}
	int M_OpenReadOnly(const char* path) {
		int handle;
#ifdef _MSC_VER 
		errno_t err = _sopen_s(&handle, path, _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
		assert(err == 0);
#else
		handle = _open(name().data(), _O_RDONLY | _O_BINARY);

#endif
		return handle;
	}

	size_t M_FileSize(int handle){
		struct stat st;
		fstat(handle, &st);
		return st.st_size;
	}

	int M_Seek(int handle, SeekPos origin, std::ptrdiff_t offset) {
		return _lseek(handle, offset, origin == SeekPos::Begin ? SEEK_SET : origin == SeekPos::Current ? SEEK_CUR : SEEK_END );
	}
	int M_Read(int handle, void* buffer, size_t len) {
		return _read(handle, buffer, len);
	}
	void M_Close(int handle) {
		_close(handle);
	}
	class PosixFileStream : public FileStream {
	public:
		enum class Mode {
			ReadOnly = 0,
			WriteOnly = 1,
			ReadWrite = 2,
		};
		static void* operator new(size_t size) {
			return Z_Malloc(size, PU::STATIC, nullptr);  
		}
		static void operator delete(void *ptr) {
			Z_Free(ptr);
		}
		size_t read(char* data, size_t count) override final;
		size_t write(const char* data, size_t count) override final;
		bool open() override final;
		void close() override final;
		PosixFileStream(string_view name, Mode mode);

		size_t offset() const { return _offset; }
		string_view name() const override final { return _name; }
		size_t size() const { return _size; }
		void offset(size_t i)  override final { _offset = std::min(_offset, _size); }
		bool readonly() const override final { return _mode == 0; }
		friend 	FileStream* I_LoadFileStream(const char* path);
	private:
		string_view _name;
		int _mode;
		size_t _offset;
		size_t _size;
		int _handle;
	};

	size_t PosixFileStream::read(char* data, size_t count) {
		assert(_handle != -1);
		if (_handle == -1) return 0U;
		int len = _read(_handle, data, (unsigned int)count);
		if (len < 0) len = 0;
		return (size_t)len;
	}
	size_t PosixFileStream::write(const char* data, size_t count) {
		assert(_handle != -1);
		if (_handle == -1) return 0U;
		int len = _write(_handle, data, (unsigned int)count);
		if (len < 0) len = 0;
		return (size_t)len;
	}
	bool PosixFileStream::open() {
		if (_handle != -1) {
#ifdef _MSC_VER 
			errno_t err = _sopen_s(&_handle,name().data(), _mode, _SH_DENYNO, _S_IREAD | _S_IWRITE);
			assert(err == 0);
#else
			_handle = _open(name().data(), _mode);
#endif
			if (_handle == -1) return false;
		}
		return true;
	}
	void PosixFileStream::close() {
		if (_handle != -1) {
			_close(_handle);
			_handle = -1;
		}
	}
	PosixFileStream::PosixFileStream(string_view name, Mode mode) : _name(name), _offset(0), _handle(-1) {
		struct stat st;
		assert(stat(_name.data(), &st) == 0);
		_size = st.st_size;
		switch (mode) {
		case Mode::ReadOnly:
				_mode = _O_RDONLY | _O_BINARY;
				break;
		case Mode::WriteOnly:
			_mode = _O_CREAT | _O_BINARY | _O_RDWR;
			break;
		case Mode::ReadWrite:
			_mode = _O_CREAT | _O_BINARY | _O_RDWR;
			break;
		}
	}

	FileStream* I_LoadFileStream(const char* path) {
		//return Z_Malloc(size, PU::STATIC, nullptr);
		return new PosixFileStream(path, PosixFileStream::Mode::ReadWrite);
	}
};
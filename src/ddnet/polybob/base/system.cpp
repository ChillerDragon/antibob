#include "system.h"

#include "lock.h"
#include "logger.h"

#include <sys/types.h>

#include <atomic>
#include <cctype>
#include <charconv>
#include <chrono>
#include <cinttypes>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iomanip> // std::get_time
#include <iterator> // std::size
#include <sstream> // std::istringstream
#include <string_view>

#if defined(CONF_WEBSOCKETS)
#include <engine/shared/websockets.h>
#endif

#if defined(CONF_FAMILY_UNIX)
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <locale>

/* unix net includes */
#include <arpa/inet.h>
#include <dirent.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <cerrno>

#if defined(CONF_PLATFORM_MACOS)
// some lock and pthread functions are already defined in headers
// included from Carbon.h
// this prevents having duplicate definitions of those
#define _lock_set_user_
#define _task_user_

#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#include <mach-o/dyld.h>
#include <mach/mach_time.h>

#if defined(__MAC_10_10) && __MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_10
#include <pthread/qos.h>
#endif
#endif

#elif defined(CONF_FAMILY_WINDOWS)
#include <io.h>
#include <objbase.h>
#include <process.h>
#include <share.h>
#include <shellapi.h>
#include <shlobj.h> // SHChangeNotify
#include <shlwapi.h>
#include <wincrypt.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <cerrno>
#include <cfenv>
#else
#error NOT IMPLEMENTED
#endif

namespace polybob
{

	std::atomic_bool dbg_assert_failing = false;
	DBG_ASSERT_HANDLER dbg_assert_handler;

	bool dbg_assert_has_failed()
	{
		return dbg_assert_failing.load(std::memory_order_acquire);
	}

	void dbg_assert_imp(const char *filename, int line, const char *fmt, ...)
	{
		const bool already_failing = dbg_assert_has_failed();
		dbg_assert_failing.store(true, std::memory_order_release);
		char msg[512];
		va_list args;
		va_start(args, fmt);
		str_format_v(msg, sizeof(msg), fmt, args);
		char error[512];
		str_format(error, sizeof(error), "%s(%d): %s", filename, line, msg);
		va_end(args);
		// this is nasty because it will not write to the logfile
		// it should probably write to some dedicated antibob_asserts.txt file
		fprintf(stderr, "[antibob-assert] %s\n", error);
		// log_error("assert", "%s", error);
		if(!already_failing)
		{
			DBG_ASSERT_HANDLER handler = dbg_assert_handler;
			if(handler)
				handler(error);
		}
		dbg_break();
	}

	void dbg_break()
	{
#ifdef __GNUC__
		__builtin_trap();
#else
		abort();
#endif
	}

	void dbg_assert_set_handler(DBG_ASSERT_HANDLER handler)
	{
		dbg_assert_handler = std::move(handler);
	}

	void dbg_msg(const char *sys, const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		log_log_v(LEVEL_INFO, sys, fmt, args);
		va_end(args);
	}

	void mem_copy(void *dest, const void *source, size_t size)
	{
		memcpy(dest, source, size);
	}

	void mem_move(void *dest, const void *source, size_t size)
	{
		memmove(dest, source, size);
	}

	int mem_comp(const void *a, const void *b, size_t size)
	{
		return memcmp(a, b, size);
	}

	bool mem_has_null(const void *block, size_t size)
	{
		const unsigned char *bytes = (const unsigned char *)block;
		for(size_t i = 0; i < size; i++)
		{
			if(bytes[i] == 0)
			{
				return true;
			}
		}
		return false;
	}

	IOHANDLE io_open(const char *filename, int flags)
	{
		dbg_assert(flags == IOFLAG_READ || flags == IOFLAG_WRITE || flags == IOFLAG_APPEND, "flags must be read, write or append");
#if defined(CONF_FAMILY_WINDOWS)
		const std::wstring wide_filename = windows_utf8_to_wide(filename);
		DWORD desired_access;
		DWORD creation_disposition;
		const char *open_mode;
		if((flags & IOFLAG_READ) != 0)
		{
			desired_access = FILE_READ_DATA;
			creation_disposition = OPEN_EXISTING;
			open_mode = "rb";
		}
		else if(flags == IOFLAG_WRITE)
		{
			desired_access = FILE_WRITE_DATA;
			creation_disposition = CREATE_ALWAYS;
			open_mode = "wb";
		}
		else if(flags == IOFLAG_APPEND)
		{
			desired_access = FILE_APPEND_DATA;
			creation_disposition = OPEN_ALWAYS;
			open_mode = "ab";
		}
		else
		{
			dbg_assert(false, "logic error");
			return nullptr;
		}
		HANDLE handle = CreateFileW(wide_filename.c_str(), desired_access, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, creation_disposition, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(handle == INVALID_HANDLE_VALUE)
			return nullptr;
		const int file_descriptor = _open_osfhandle((intptr_t)handle, 0);
		dbg_assert(file_descriptor != -1, "_open_osfhandle failure");
		FILE *file_stream = _fdopen(file_descriptor, open_mode);
		dbg_assert(file_stream != nullptr, "_fdopen failure");
		return file_stream;
#else
		const char *open_mode;
		if((flags & IOFLAG_READ) != 0)
		{
			open_mode = "rb";
		}
		else if(flags == IOFLAG_WRITE)
		{
			open_mode = "wb";
		}
		else if(flags == IOFLAG_APPEND)
		{
			open_mode = "ab";
		}
		else
		{
			dbg_assert(false, "logic error");
			return nullptr;
		}
		return fopen(filename, open_mode);
#endif
	}

	unsigned io_read(IOHANDLE io, void *buffer, unsigned size)
	{
		return fread(buffer, 1, size, (FILE *)io);
	}

	bool io_read_all(IOHANDLE io, void **result, unsigned *result_len)
	{
		// Loading files larger than 1 GiB into memory is not supported.
		constexpr int64_t MAX_FILE_SIZE = (int64_t)1024 * 1024 * 1024;

		int64_t real_len = io_length(io);
		if(real_len > MAX_FILE_SIZE)
		{
			*result = nullptr;
			*result_len = 0;
			return false;
		}

		int64_t len = real_len < 0 ? 1024 : real_len; // use default initial size if we couldn't get the length
		char *buffer = (char *)malloc(len + 1);
		int64_t read = io_read(io, buffer, len + 1); // +1 to check if the file size is larger than expected
		if(read < len)
		{
			buffer = (char *)realloc(buffer, read + 1);
			len = read;
		}
		else if(read > len)
		{
			int64_t cap = 2 * read;
			if(cap > MAX_FILE_SIZE)
			{
				free(buffer);
				*result = nullptr;
				*result_len = 0;
				return false;
			}
			len = read;
			buffer = (char *)realloc(buffer, cap);
			while((read = io_read(io, buffer + len, cap - len)) != 0)
			{
				len += read;
				if(len == cap)
				{
					cap *= 2;
					if(cap > MAX_FILE_SIZE)
					{
						free(buffer);
						*result = nullptr;
						*result_len = 0;
						return false;
					}
					buffer = (char *)realloc(buffer, cap);
				}
			}
			buffer = (char *)realloc(buffer, len + 1);
		}
		buffer[len] = 0;
		*result = buffer;
		*result_len = len;
		return true;
	}

	char *io_read_all_str(IOHANDLE io)
	{
		void *buffer;
		unsigned len;
		if(!io_read_all(io, &buffer, &len))
		{
			return nullptr;
		}
		if(mem_has_null(buffer, len))
		{
			free(buffer);
			return nullptr;
		}
		return (char *)buffer;
	}

	int io_skip(IOHANDLE io, int64_t size)
	{
		return io_seek(io, size, IOSEEK_CUR);
	}

	int io_seek(IOHANDLE io, int64_t offset, ESeekOrigin origin)
	{
		int real_origin;
		switch(origin)
		{
		case IOSEEK_START:
			real_origin = SEEK_SET;
			break;
		case IOSEEK_CUR:
			real_origin = SEEK_CUR;
			break;
		case IOSEEK_END:
			real_origin = SEEK_END;
			break;
		default:
			dbg_assert(false, "origin invalid");
			return -1;
		}
#if defined(CONF_FAMILY_WINDOWS)
		return _fseeki64((FILE *)io, offset, real_origin);
#else
		return fseeko((FILE *)io, offset, real_origin);
#endif
	}

	int64_t io_tell(IOHANDLE io)
	{
#if defined(CONF_FAMILY_WINDOWS)
		return _ftelli64((FILE *)io);
#else
		return ftello((FILE *)io);
#endif
	}

	int64_t io_length(IOHANDLE io)
	{
		if(io_seek(io, 0, IOSEEK_END) != 0)
		{
			return -1;
		}
		const int64_t length = io_tell(io);
		if(io_seek(io, 0, IOSEEK_START) != 0)
		{
			return -1;
		}
		return length;
	}

	unsigned io_write(IOHANDLE io, const void *buffer, unsigned size)
	{
		return fwrite(buffer, 1, size, (FILE *)io);
	}

	bool io_write_newline(IOHANDLE io)
	{
#if defined(CONF_FAMILY_WINDOWS)
		return io_write(io, "\r\n", 2) == 2;
#else
		return io_write(io, "\n", 1) == 1;
#endif
	}

	int io_close(IOHANDLE io)
	{
		return fclose((FILE *)io) != 0;
	}

	int io_flush(IOHANDLE io)
	{
		return fflush((FILE *)io);
	}

	int io_sync(IOHANDLE io)
	{
		if(io_flush(io))
		{
			return 1;
		}
#if defined(CONF_FAMILY_WINDOWS)
		return FlushFileBuffers((HANDLE)_get_osfhandle(_fileno((FILE *)io))) == FALSE;
#else
		return fsync(fileno((FILE *)io)) != 0;
#endif
	}

	int io_error(IOHANDLE io)
	{
		return ferror((FILE *)io);
	}

	IOHANDLE io_stdin()
	{
		return stdin;
	}

	IOHANDLE io_stdout()
	{
		return stdout;
	}

	IOHANDLE io_stderr()
	{
		return stderr;
	}

	IOHANDLE io_current_exe()
	{
		// From https://stackoverflow.com/a/1024937.
#if defined(CONF_FAMILY_WINDOWS)
		wchar_t wide_path[IO_MAX_PATH_LENGTH];
		if(GetModuleFileNameW(nullptr, wide_path, std::size(wide_path)) == 0 || GetLastError() != ERROR_SUCCESS)
		{
			return nullptr;
		}
		const std::optional<std::string> path = windows_wide_to_utf8(wide_path);
		return path.has_value() ? io_open(path.value().c_str(), IOFLAG_READ) : nullptr;
#elif defined(CONF_PLATFORM_MACOS)
		char path[IO_MAX_PATH_LENGTH];
		uint32_t path_size = sizeof(path);
		if(_NSGetExecutablePath(path, &path_size))
		{
			return 0;
		}
		return io_open(path, IOFLAG_READ);
#else
		static const char *NAMES[] = {
			"/proc/self/exe", // Linux, Android
			"/proc/curproc/exe", // NetBSD
			"/proc/curproc/file", // DragonFly
		};
		for(auto &name : NAMES)
		{
			IOHANDLE result = io_open(name, IOFLAG_READ);
			if(result)
			{
				return result;
			}
		}
		return 0;
#endif
	}

#define ASYNC_BUFSIZE (8 * 1024)
#define ASYNC_LOCAL_BUFSIZE (64 * 1024)

	struct ASYNCIO
	{
		CLock lock;
		IOHANDLE io;
		SEMAPHORE sphore;
		void *thread;

		unsigned char *buffer;
		unsigned int buffer_size;
		unsigned int read_pos;
		unsigned int write_pos;

		int error;
		unsigned char finish;
		unsigned char refcount;
	};

	enum
	{
		ASYNCIO_RUNNING,
		ASYNCIO_CLOSE,
		ASYNCIO_EXIT,
	};

	struct BUFFERS
	{
		unsigned char *buf1;
		unsigned int len1;
		unsigned char *buf2;
		unsigned int len2;
	};

	static void buffer_ptrs(ASYNCIO *aio, struct BUFFERS *buffers)
	{
		mem_zero(buffers, sizeof(*buffers));
		if(aio->read_pos < aio->write_pos)
		{
			buffers->buf1 = aio->buffer + aio->read_pos;
			buffers->len1 = aio->write_pos - aio->read_pos;
		}
		else if(aio->read_pos > aio->write_pos)
		{
			buffers->buf1 = aio->buffer + aio->read_pos;
			buffers->len1 = aio->buffer_size - aio->read_pos;
			buffers->buf2 = aio->buffer;
			buffers->len2 = aio->write_pos;
		}
	}

	static void aio_handle_free_and_unlock(ASYNCIO *aio) RELEASE(aio->lock)
	{
		int do_free;
		aio->refcount--;

		do_free = aio->refcount == 0;
		aio->lock.unlock();
		if(do_free)
		{
			free(aio->buffer);
			sphore_destroy(&aio->sphore);
			delete aio;
		}
	}

	static int new_tick = -1;

	void set_new_tick()
	{
		new_tick = 1;
	}

	/* -----  time ----- */
	static_assert(std::chrono::steady_clock::is_steady, "Compiler does not support steady clocks, it might be out of date.");
	static_assert(std::chrono::steady_clock::period::den / std::chrono::steady_clock::period::num >= 1000000000, "Compiler has a bad timer precision and might be out of date.");
	static const std::chrono::time_point<std::chrono::steady_clock> tw_start_time = std::chrono::steady_clock::now();

	int64_t time_get_impl()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - tw_start_time).count();
	}

	int64_t time_get()
	{
		static int64_t last = 0;
		if(new_tick == 0)
			return last;
		if(new_tick != -1)
			new_tick = 0;

		last = time_get_impl();
		return last;
	}

	int64_t time_freq()
	{
		using namespace std::chrono_literals;
		return std::chrono::nanoseconds(1s).count();
	}

	int str_length(const char *str)
	{
		return (int)strlen(str);
	}

	int str_format_v(char *buffer, int buffer_size, const char *format, va_list args)
	{
#if defined(CONF_FAMILY_WINDOWS)
		_vsprintf_p(buffer, buffer_size, format, args);
		buffer[buffer_size - 1] = 0; /* assure null termination */
#else
		vsnprintf(buffer, buffer_size, format, args);
		/* null termination is assured by definition of vsnprintf */
#endif
		return str_utf8_fix_truncation(buffer);
	}

	int str_format_int(char *buffer, size_t buffer_size, int value)
	{
		buffer[0] = '\0'; // Fix false positive clang-analyzer-core.UndefinedBinaryOperatorResult when using result
		auto result = std::to_chars(buffer, buffer + buffer_size - 1, value);
		result.ptr[0] = '\0';
		return result.ptr - buffer;
	}

#undef str_format
	int str_format(char *buffer, int buffer_size, const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		int length = str_format_v(buffer, buffer_size, format, args);
		va_end(args);
		return length;
	}
#if !defined(CONF_DEBUG)
#define str_format str_format_opt
#endif

	void str_sanitize_cc(char *str_in)
	{
		unsigned char *str = (unsigned char *)str_in;
		while(*str)
		{
			if(*str < 32)
				*str = ' ';
			str++;
		}
	}

	/* makes sure that the string only contains the characters between 32 and 255 + \r\n\t */
	void str_sanitize(char *str_in)
	{
		unsigned char *str = (unsigned char *)str_in;
		while(*str)
		{
			if(*str < 32 && !(*str == '\r') && !(*str == '\n') && !(*str == '\t'))
				*str = ' ';
			str++;
		}
	}

	int str_isspace(char c)
	{
		return c == ' ' || c == '\n' || c == '\r' || c == '\t';
	}

	char str_uppercase(char c)
	{
		if(c >= 'a' && c <= 'z')
			return 'A' + (c - 'a');
		return c;
	}

	bool str_isnum(char c)
	{
		return c >= '0' && c <= '9';
	}

	int str_isallnum(const char *str)
	{
		while(*str)
		{
			if(!str_isnum(*str))
				return 0;
			str++;
		}
		return 1;
	}

	int str_isallnum_hex(const char *str)
	{
		while(*str)
		{
			if(!str_isnum(*str) && !(*str >= 'a' && *str <= 'f') && !(*str >= 'A' && *str <= 'F'))
				return 0;
			str++;
		}
		return 1;
	}

	int str_toint(const char *str)
	{
		return str_toint_base(str, 10);
	}

	bool str_toint(const char *str, int *out)
	{
		// returns true if conversion was successful
		char *end;
		int value = strtol(str, &end, 10);
		if(*end != '\0')
			return false;
		if(out != nullptr)
			*out = value;
		return true;
	}

	int str_toint_base(const char *str, int base)
	{
		return strtol(str, nullptr, base);
	}

	unsigned long str_toulong_base(const char *str, int base)
	{
		return strtoul(str, nullptr, base);
	}

	int64_t str_toint64_base(const char *str, int base)
	{
		return strtoll(str, nullptr, base);
	}

	float str_tofloat(const char *str)
	{
		return strtod(str, nullptr);
	}

	bool str_tofloat(const char *str, float *out)
	{
		// returns true if conversion was successful
		char *end;
		float value = strtod(str, &end);
		if(*end != '\0')
			return false;
		if(out != nullptr)
			*out = value;
		return true;
	}

	int str_comp_nocase(const char *a, const char *b)
	{
#if defined(CONF_FAMILY_WINDOWS)
		return _stricmp(a, b);
#else
		return strcasecmp(a, b);
#endif
	}

	int str_comp_nocase_num(const char *a, const char *b, int num)
	{
#if defined(CONF_FAMILY_WINDOWS)
		return _strnicmp(a, b, num);
#else
		return strncasecmp(a, b, num);
#endif
	}

	int str_comp(const char *a, const char *b)
	{
		return strcmp(a, b);
	}

	int str_comp_num(const char *a, const char *b, int num)
	{
		return strncmp(a, b, num);
	}

	int str_comp_filenames(const char *a, const char *b)
	{
		int result;

		for(; *a && *b; ++a, ++b)
		{
			if(str_isnum(*a) && str_isnum(*b))
			{
				result = 0;
				do
				{
					if(!result)
						result = *a - *b;
					++a;
					++b;
				} while(str_isnum(*a) && str_isnum(*b));

				if(str_isnum(*a))
					return 1;
				else if(str_isnum(*b))
					return -1;
				else if(result || *a == '\0' || *b == '\0')
					return result;
			}

			result = tolower(*a) - tolower(*b);
			if(result)
				return result;
		}
		return *a - *b;
	}

	const char *str_startswith_nocase(const char *str, const char *prefix)
	{
		int prefixl = str_length(prefix);
		if(str_comp_nocase_num(str, prefix, prefixl) == 0)
		{
			return str + prefixl;
		}
		else
		{
			return nullptr;
		}
	}

	const char *str_startswith(const char *str, const char *prefix)
	{
		int prefixl = str_length(prefix);
		if(str_comp_num(str, prefix, prefixl) == 0)
		{
			return str + prefixl;
		}
		else
		{
			return nullptr;
		}
	}

	const char *str_endswith_nocase(const char *str, const char *suffix)
	{
		int strl = str_length(str);
		int suffixl = str_length(suffix);
		const char *strsuffix;
		if(strl < suffixl)
		{
			return nullptr;
		}
		strsuffix = str + strl - suffixl;
		if(str_comp_nocase(strsuffix, suffix) == 0)
		{
			return strsuffix;
		}
		else
		{
			return nullptr;
		}
	}

	const char *str_endswith(const char *str, const char *suffix)
	{
		int strl = str_length(str);
		int suffixl = str_length(suffix);
		const char *strsuffix;
		if(strl < suffixl)
		{
			return nullptr;
		}
		strsuffix = str + strl - suffixl;
		if(str_comp(strsuffix, suffix) == 0)
		{
			return strsuffix;
		}
		else
		{
			return nullptr;
		}
	}

	const char *str_find_nocase(const char *haystack, const char *needle)
	{
		while(*haystack) /* native implementation */
		{
			const char *a = haystack;
			const char *b = needle;
			while(*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b))
			{
				a++;
				b++;
			}
			if(!(*b))
				return haystack;
			haystack++;
		}

		return nullptr;
	}

	const char *str_find(const char *haystack, const char *needle)
	{
		while(*haystack) /* native implementation */
		{
			const char *a = haystack;
			const char *b = needle;
			while(*a && *b && *a == *b)
			{
				a++;
				b++;
			}
			if(!(*b))
				return haystack;
			haystack++;
		}

		return nullptr;
	}

	void str_hex(char *dst, int dst_size, const void *data, int data_size)
	{
		static const char hex[] = "0123456789ABCDEF";
		int data_index;
		int dst_index;
		for(data_index = 0, dst_index = 0; data_index < data_size && dst_index < dst_size - 3; data_index++)
		{
			dst[data_index * 3] = hex[((const unsigned char *)data)[data_index] >> 4];
			dst[data_index * 3 + 1] = hex[((const unsigned char *)data)[data_index] & 0xf];
			dst[data_index * 3 + 2] = ' ';
			dst_index += 3;
		}
		dst[dst_index] = '\0';
	}

	void str_hex_cstyle(char *dst, int dst_size, const void *data, int data_size, int bytes_per_line)
	{
		static const char hex[] = "0123456789ABCDEF";
		int data_index;
		int dst_index;
		int remaining_bytes_per_line = bytes_per_line;
		for(data_index = 0, dst_index = 0; data_index < data_size && dst_index < dst_size - 6; data_index++)
		{
			--remaining_bytes_per_line;
			dst[data_index * 6] = '0';
			dst[data_index * 6 + 1] = 'x';
			dst[data_index * 6 + 2] = hex[((const unsigned char *)data)[data_index] >> 4];
			dst[data_index * 6 + 3] = hex[((const unsigned char *)data)[data_index] & 0xf];
			dst[data_index * 6 + 4] = ',';
			if(remaining_bytes_per_line == 0)
			{
				dst[data_index * 6 + 5] = '\n';
				remaining_bytes_per_line = bytes_per_line;
			}
			else
			{
				dst[data_index * 6 + 5] = ' ';
			}
			dst_index += 6;
		}
		dst[dst_index] = '\0';
		// Remove trailing comma and space/newline
		if(dst_index >= 1)
			dst[dst_index - 1] = '\0';
		if(dst_index >= 2)
			dst[dst_index - 2] = '\0';
	}

	static int hexval(char x)
	{
		switch(x)
		{
		case '0': return 0;
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		case 'a':
		case 'A': return 10;
		case 'b':
		case 'B': return 11;
		case 'c':
		case 'C': return 12;
		case 'd':
		case 'D': return 13;
		case 'e':
		case 'E': return 14;
		case 'f':
		case 'F': return 15;
		default: return -1;
		}
	}

	static int byteval(const char *hex, unsigned char *dst)
	{
		int v1 = hexval(hex[0]);
		int v2 = hexval(hex[1]);

		if(v1 < 0 || v2 < 0)
			return 1;

		*dst = v1 * 16 + v2;
		return 0;
	}

	int str_hex_decode(void *dst, int dst_size, const char *src)
	{
		unsigned char *cdst = (unsigned char *)dst;
		int slen = str_length(src);
		int len = slen / 2;
		int i;
		if(slen != dst_size * 2)
			return 2;

		for(i = 0; i < len && dst_size; i++, dst_size--)
		{
			if(byteval(src + i * 2, cdst++))
				return 1;
		}
		return 0;
	}

	void str_base64(char *dst, int dst_size, const void *data_raw, int data_size)
	{
		static const char DIGITS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		const unsigned char *data = (const unsigned char *)data_raw;
		unsigned value = 0;
		int num_bits = 0;
		int i = 0;
		int o = 0;

		dst_size -= 1;
		dst[dst_size] = 0;
		while(true)
		{
			if(num_bits < 6 && i < data_size)
			{
				value = (value << 8) | data[i];
				num_bits += 8;
				i += 1;
			}
			if(o == dst_size)
			{
				return;
			}
			if(num_bits > 0)
			{
				unsigned padded;
				if(num_bits >= 6)
				{
					padded = (value >> (num_bits - 6)) & 0x3f;
				}
				else
				{
					padded = (value << (6 - num_bits)) & 0x3f;
				}
				dst[o] = DIGITS[padded];
				num_bits -= 6;
				o += 1;
			}
			else if(o % 4 != 0)
			{
				dst[o] = '=';
				o += 1;
			}
			else
			{
				dst[o] = 0;
				return;
			}
		}
	}

	static int base64_digit_value(char digit)
	{
		if('A' <= digit && digit <= 'Z')
		{
			return digit - 'A';
		}
		else if('a' <= digit && digit <= 'z')
		{
			return digit - 'a' + 26;
		}
		else if('0' <= digit && digit <= '9')
		{
			return digit - '0' + 52;
		}
		else if(digit == '+')
		{
			return 62;
		}
		else if(digit == '/')
		{
			return 63;
		}
		return -1;
	}

	int str_base64_decode(void *dst_raw, int dst_size, const char *data)
	{
		unsigned char *dst = (unsigned char *)dst_raw;
		int data_len = str_length(data);

		int i;
		int o = 0;

		if(data_len % 4 != 0)
		{
			return -3;
		}
		if(data_len / 4 * 3 > dst_size)
		{
			// Output buffer too small.
			return -2;
		}
		for(i = 0; i < data_len; i += 4)
		{
			int num_output_bytes = 3;
			char copy[4];
			int d[4];
			int value;
			int b;
			mem_copy(copy, data + i, sizeof(copy));
			if(i == data_len - 4)
			{
				if(copy[3] == '=')
				{
					copy[3] = 'A';
					num_output_bytes = 2;
					if(copy[2] == '=')
					{
						copy[2] = 'A';
						num_output_bytes = 1;
					}
				}
			}
			d[0] = base64_digit_value(copy[0]);
			d[1] = base64_digit_value(copy[1]);
			d[2] = base64_digit_value(copy[2]);
			d[3] = base64_digit_value(copy[3]);
			if(d[0] == -1 || d[1] == -1 || d[2] == -1 || d[3] == -1)
			{
				// Invalid digit.
				return -1;
			}
			value = (d[0] << 18) | (d[1] << 12) | (d[2] << 6) | d[3];
			for(b = 0; b < 3; b++)
			{
				unsigned char byte_value = (value >> (16 - 8 * b)) & 0xff;
				if(b < num_output_bytes)
				{
					dst[o] = byte_value;
					o += 1;
				}
				else
				{
					if(byte_value != 0)
					{
						// Padding not zeroed.
						return -2;
					}
				}
			}
		}
		return o;
	}

	void str_append(char *dst, const char *src, int dst_size)
	{
		int s = str_length(dst);
		int i = 0;
		while(s < dst_size)
		{
			dst[s] = src[i];
			if(!src[i]) /* check for null termination */
				break;
			s++;
			i++;
		}

		dst[dst_size - 1] = 0; /* assure null termination */
		str_utf8_fix_truncation(dst);
	}

	int str_copy(char *dst, const char *src, int dst_size)
	{
		dst[0] = '\0';
		strncat(dst, src, dst_size - 1);
		return str_utf8_fix_truncation(dst);
	}

	static struct tm *time_localtime_threadlocal(time_t *time_data)
	{
#if defined(CONF_FAMILY_WINDOWS)
		// The result of localtime is thread-local on Windows
		// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/localtime-localtime32-localtime64
		return localtime(time_data);
#else
		// Thread-local buffer for the result of localtime_r
		thread_local struct tm time_info_buf;
		return localtime_r(time_data, &time_info_buf);
#endif
	}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
	void str_timestamp_ex(time_t time_data, char *buffer, int buffer_size, const char *format)
	{
		struct tm *time_info = time_localtime_threadlocal(&time_data);
		strftime(buffer, buffer_size, format, time_info);
		buffer[buffer_size - 1] = 0; /* assure null termination */
	}

	void str_timestamp_format(char *buffer, int buffer_size, const char *format)
	{
		time_t time_data;
		time(&time_data);
		str_timestamp_ex(time_data, buffer, buffer_size, format);
	}

	void str_timestamp(char *buffer, int buffer_size)
	{
		str_timestamp_format(buffer, buffer_size, FORMAT_NOSPACE);
	}

	bool timestamp_from_str(const char *string, const char *format, time_t *timestamp)
	{
		std::tm tm{};
		std::istringstream ss(string);
		ss >> std::get_time(&tm, format);
		if(ss.fail() || !ss.eof())
			return false;

		time_t result = mktime(&tm);
		if(result < 0)
			return false;

		*timestamp = result;
		return true;
	}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

	int str_time(int64_t centisecs, int format, char *buffer, int buffer_size)
	{
		const int sec = 100;
		const int min = 60 * sec;
		const int hour = 60 * min;
		const int day = 24 * hour;

		if(buffer_size <= 0)
			return -1;

		if(centisecs < 0)
			centisecs = 0;

		buffer[0] = 0;

		switch(format)
		{
		case TIME_DAYS:
			if(centisecs >= day)
				return str_format(buffer, buffer_size, "%" PRId64 "d %02" PRId64 ":%02" PRId64 ":%02" PRId64, centisecs / day,
					(centisecs % day) / hour, (centisecs % hour) / min, (centisecs % min) / sec);
			[[fallthrough]];
		case TIME_HOURS:
			if(centisecs >= hour)
				return str_format(buffer, buffer_size, "%02" PRId64 ":%02" PRId64 ":%02" PRId64, centisecs / hour,
					(centisecs % hour) / min, (centisecs % min) / sec);
			[[fallthrough]];
		case TIME_MINS:
			return str_format(buffer, buffer_size, "%02" PRId64 ":%02" PRId64, centisecs / min,
				(centisecs % min) / sec);
		case TIME_HOURS_CENTISECS:
			if(centisecs >= hour)
				return str_format(buffer, buffer_size, "%02" PRId64 ":%02" PRId64 ":%02" PRId64 ".%02" PRId64, centisecs / hour,
					(centisecs % hour) / min, (centisecs % min) / sec, centisecs % sec);
			[[fallthrough]];
		case TIME_MINS_CENTISECS:
			if(centisecs >= min)
				return str_format(buffer, buffer_size, "%02" PRId64 ":%02" PRId64 ".%02" PRId64, centisecs / min,
					(centisecs % min) / sec, centisecs % sec);
			[[fallthrough]];
		case TIME_SECS_CENTISECS:
			return str_format(buffer, buffer_size, "%02" PRId64 ".%02" PRId64, (centisecs % min) / sec, centisecs % sec);
		}

		return -1;
	}

	int str_time_float(float secs, int format, char *buffer, int buffer_size)
	{
		return str_time(llroundf(secs * 1000) / 10, format, buffer, buffer_size);
	}

	void str_escape(char **dst, const char *src, const char *end)
	{
		while(*src && *dst + 1 < end)
		{
			if(*src == '"' || *src == '\\') // escape \ and "
			{
				if(*dst + 2 < end)
					*(*dst)++ = '\\';
				else
					break;
			}
			*(*dst)++ = *src++;
		}
		**dst = 0;
	}

	int str_utf8_comp_nocase(const char *a, const char *b)
	{
		int code_a;
		int code_b;

		while(*a && *b)
		{
			code_a = str_utf8_tolower(str_utf8_decode(&a));
			code_b = str_utf8_tolower(str_utf8_decode(&b));

			if(code_a != code_b)
				return code_a - code_b;
		}
		return (unsigned char)*a - (unsigned char)*b;
	}

	int str_utf8_comp_nocase_num(const char *a, const char *b, int num)
	{
		int code_a;
		int code_b;
		const char *old_a = a;

		if(num <= 0)
			return 0;

		while(*a && *b)
		{
			code_a = str_utf8_tolower(str_utf8_decode(&a));
			code_b = str_utf8_tolower(str_utf8_decode(&b));

			if(code_a != code_b)
				return code_a - code_b;

			if(a - old_a >= num)
				return 0;
		}

		return (unsigned char)*a - (unsigned char)*b;
	}

	const char *str_utf8_find_nocase(const char *haystack, const char *needle, const char **end)
	{
		while(*haystack) /* native implementation */
		{
			const char *a = haystack;
			const char *b = needle;
			const char *a_next = a;
			const char *b_next = b;
			while(*a && *b && str_utf8_tolower(str_utf8_decode(&a_next)) == str_utf8_tolower(str_utf8_decode(&b_next)))
			{
				a = a_next;
				b = b_next;
			}
			if(!(*b))
			{
				if(end != nullptr)
					*end = a_next;
				return haystack;
			}
			str_utf8_decode(&haystack);
		}

		if(end != nullptr)
			*end = nullptr;
		return nullptr;
	}

	int str_utf8_isspace(int code)
	{
		return code <= 0x0020 || code == 0x0085 || code == 0x00A0 || code == 0x034F ||
		       code == 0x115F || code == 0x1160 || code == 0x1680 || code == 0x180E ||
		       (code >= 0x2000 && code <= 0x200F) || (code >= 0x2028 && code <= 0x202F) ||
		       (code >= 0x205F && code <= 0x2064) || (code >= 0x206A && code <= 0x206F) ||
		       code == 0x2800 || code == 0x3000 || code == 0x3164 ||
		       (code >= 0xFE00 && code <= 0xFE0F) || code == 0xFEFF || code == 0xFFA0 ||
		       (code >= 0xFFF9 && code <= 0xFFFC);
	}

	const char *str_utf8_skip_whitespaces(const char *str)
	{
		const char *str_old;
		int code;

		while(*str)
		{
			str_old = str;
			code = str_utf8_decode(&str);

			// check if unicode is not empty
			if(!str_utf8_isspace(code))
			{
				return str_old;
			}
		}

		return str;
	}

	void str_utf8_trim_right(char *param)
	{
		const char *str = param;
		char *end = nullptr;
		while(*str)
		{
			char *str_old = (char *)str;
			int code = str_utf8_decode(&str);

			// check if unicode is not empty
			if(!str_utf8_isspace(code))
			{
				end = nullptr;
			}
			else if(!end)
			{
				end = str_old;
			}
		}
		if(end)
		{
			*end = 0;
		}
	}

	int str_utf8_isstart(char c)
	{
		if((c & 0xC0) == 0x80) /* 10xxxxxx */
			return 0;
		return 1;
	}

	int str_utf8_rewind(const char *str, int cursor)
	{
		while(cursor)
		{
			cursor--;
			if(str_utf8_isstart(*(str + cursor)))
				break;
		}
		return cursor;
	}

	int str_utf8_fix_truncation(char *str)
	{
		int len = str_length(str);
		if(len > 0)
		{
			int last_char_index = str_utf8_rewind(str, len);
			const char *last_char = str + last_char_index;
			// Fix truncated UTF-8.
			if(str_utf8_decode(&last_char) == -1)
			{
				str[last_char_index] = 0;
				return last_char_index;
			}
		}
		return len;
	}

	int str_utf8_forward(const char *str, int cursor)
	{
		const char *ptr = str + cursor;
		if(str_utf8_decode(&ptr) == 0)
		{
			return cursor;
		}
		return ptr - str;
	}

	int str_utf8_encode(char *ptr, int chr)
	{
		/* encode */
		if(chr <= 0x7F)
		{
			ptr[0] = (char)chr;
			return 1;
		}
		else if(chr <= 0x7FF)
		{
			ptr[0] = 0xC0 | ((chr >> 6) & 0x1F);
			ptr[1] = 0x80 | (chr & 0x3F);
			return 2;
		}
		else if(chr <= 0xFFFF)
		{
			ptr[0] = 0xE0 | ((chr >> 12) & 0x0F);
			ptr[1] = 0x80 | ((chr >> 6) & 0x3F);
			ptr[2] = 0x80 | (chr & 0x3F);
			return 3;
		}
		else if(chr <= 0x10FFFF)
		{
			ptr[0] = 0xF0 | ((chr >> 18) & 0x07);
			ptr[1] = 0x80 | ((chr >> 12) & 0x3F);
			ptr[2] = 0x80 | ((chr >> 6) & 0x3F);
			ptr[3] = 0x80 | (chr & 0x3F);
			return 4;
		}

		return 0;
	}

	static unsigned char str_byte_next(const char **ptr)
	{
		unsigned char byte_value = **ptr;
		(*ptr)++;
		return byte_value;
	}

	static void str_byte_rewind(const char **ptr)
	{
		(*ptr)--;
	}

	int str_utf8_decode(const char **ptr)
	{
		// As per https://encoding.spec.whatwg.org/#utf-8-decoder.
		unsigned char utf8_lower_boundary = 0x80;
		unsigned char utf8_upper_boundary = 0xBF;
		int utf8_code_point = 0;
		int utf8_bytes_seen = 0;
		int utf8_bytes_needed = 0;
		while(true)
		{
			unsigned char byte_value = str_byte_next(ptr);
			if(utf8_bytes_needed == 0)
			{
				if(byte_value <= 0x7F)
				{
					return byte_value;
				}
				else if(0xC2 <= byte_value && byte_value <= 0xDF)
				{
					utf8_bytes_needed = 1;
					utf8_code_point = byte_value - 0xC0;
				}
				else if(0xE0 <= byte_value && byte_value <= 0xEF)
				{
					if(byte_value == 0xE0)
						utf8_lower_boundary = 0xA0;
					if(byte_value == 0xED)
						utf8_upper_boundary = 0x9F;
					utf8_bytes_needed = 2;
					utf8_code_point = byte_value - 0xE0;
				}
				else if(0xF0 <= byte_value && byte_value <= 0xF4)
				{
					if(byte_value == 0xF0)
						utf8_lower_boundary = 0x90;
					if(byte_value == 0xF4)
						utf8_upper_boundary = 0x8F;
					utf8_bytes_needed = 3;
					utf8_code_point = byte_value - 0xF0;
				}
				else
				{
					return -1; // Error.
				}
				utf8_code_point = utf8_code_point << (6 * utf8_bytes_needed);
				continue;
			}
			if(!(utf8_lower_boundary <= byte_value && byte_value <= utf8_upper_boundary))
			{
				// Resetting variables not necessary, will be done when
				// the function is called again.
				str_byte_rewind(ptr);
				return -1;
			}
			utf8_lower_boundary = 0x80;
			utf8_upper_boundary = 0xBF;
			utf8_bytes_seen += 1;
			utf8_code_point = utf8_code_point + ((byte_value - 0x80) << (6 * (utf8_bytes_needed - utf8_bytes_seen)));
			if(utf8_bytes_seen != utf8_bytes_needed)
			{
				continue;
			}
			// Resetting variables not necessary, see above.
			return utf8_code_point;
		}
	}

	int str_utf8_check(const char *str)
	{
		int codepoint;
		while((codepoint = str_utf8_decode(&str)))
		{
			if(codepoint == -1)
			{
				return 0;
			}
		}
		return 1;
	}

	void str_utf8_copy_num(char *dst, const char *src, int dst_size, int num)
	{
		int new_cursor;
		int cursor = 0;

		while(src[cursor] && num > 0)
		{
			new_cursor = str_utf8_forward(src, cursor);
			if(new_cursor >= dst_size) // reserve 1 byte for the null termination
				break;
			else
				cursor = new_cursor;
			--num;
		}

		str_copy(dst, src, cursor < dst_size ? cursor + 1 : dst_size);
	}

	void str_utf8_stats(const char *str, size_t max_size, size_t max_count, size_t *size, size_t *count)
	{
		const char *cursor = str;
		*size = 0;
		*count = 0;
		while(*size < max_size && *count < max_count)
		{
			if(str_utf8_decode(&cursor) == 0)
			{
				break;
			}
			if((size_t)(cursor - str) >= max_size)
			{
				break;
			}
			*size = cursor - str;
			++(*count);
		}
	}

	size_t str_utf8_offset_bytes_to_chars(const char *str, size_t byte_offset)
	{
		size_t char_offset = 0;
		size_t current_offset = 0;
		while(current_offset < byte_offset)
		{
			const size_t prev_byte_offset = current_offset;
			current_offset = str_utf8_forward(str, current_offset);
			if(current_offset == prev_byte_offset)
				break;
			char_offset++;
		}
		return char_offset;
	}

	size_t str_utf8_offset_chars_to_bytes(const char *str, size_t char_offset)
	{
		size_t byte_offset = 0;
		for(size_t i = 0; i < char_offset; i++)
		{
			const size_t prev_byte_offset = byte_offset;
			byte_offset = str_utf8_forward(str, byte_offset);
			if(byte_offset == prev_byte_offset)
				break;
		}
		return byte_offset;
	}

	static void aio_thread(void *user)
	{
		ASYNCIO *aio = (ASYNCIO *)user;

		aio->lock.lock();
		while(true)
		{
			struct BUFFERS buffers;
			int result_io_error;
			unsigned char local_buffer[ASYNC_LOCAL_BUFSIZE];
			unsigned int local_buffer_len = 0;

			if(aio->read_pos == aio->write_pos)
			{
				if(aio->finish != ASYNCIO_RUNNING)
				{
					if(aio->finish == ASYNCIO_CLOSE)
					{
						io_close(aio->io);
					}
					aio_handle_free_and_unlock(aio);
					break;
				}
				aio->lock.unlock();
				sphore_wait(&aio->sphore);
				aio->lock.lock();
				continue;
			}

			buffer_ptrs(aio, &buffers);
			if(buffers.buf1)
			{
				if(buffers.len1 > sizeof(local_buffer) - local_buffer_len)
				{
					buffers.len1 = sizeof(local_buffer) - local_buffer_len;
				}
				mem_copy(local_buffer + local_buffer_len, buffers.buf1, buffers.len1);
				local_buffer_len += buffers.len1;
				if(buffers.buf2)
				{
					if(buffers.len2 > sizeof(local_buffer) - local_buffer_len)
					{
						buffers.len2 = sizeof(local_buffer) - local_buffer_len;
					}
					mem_copy(local_buffer + local_buffer_len, buffers.buf2, buffers.len2);
					local_buffer_len += buffers.len2;
				}
			}
			aio->read_pos = (aio->read_pos + buffers.len1 + buffers.len2) % aio->buffer_size;
			aio->lock.unlock();

			io_write(aio->io, local_buffer, local_buffer_len);
			io_flush(aio->io);
			result_io_error = io_error(aio->io);

			aio->lock.lock();
			aio->error = result_io_error;
		}
	}

	ASYNCIO *aio_new(IOHANDLE io)
	{
		ASYNCIO *aio = new ASYNCIO;
		if(!aio)
		{
			return nullptr;
		}
		aio->io = io;
		sphore_init(&aio->sphore);
		aio->thread = nullptr;

		aio->buffer = (unsigned char *)malloc(ASYNC_BUFSIZE);
		if(!aio->buffer)
		{
			sphore_destroy(&aio->sphore);
			delete aio;
			return nullptr;
		}
		aio->buffer_size = ASYNC_BUFSIZE;
		aio->read_pos = 0;
		aio->write_pos = 0;
		aio->error = 0;
		aio->finish = ASYNCIO_RUNNING;
		aio->refcount = 2;

		aio->thread = thread_init(aio_thread, aio, "aio");
		if(!aio->thread)
		{
			free(aio->buffer);
			sphore_destroy(&aio->sphore);
			delete aio;
			return nullptr;
		}
		return aio;
	}

	static unsigned int buffer_len(ASYNCIO *aio)
	{
		if(aio->write_pos >= aio->read_pos)
		{
			return aio->write_pos - aio->read_pos;
		}
		else
		{
			return aio->buffer_size + aio->write_pos - aio->read_pos;
		}
	}

	static unsigned int next_buffer_size(unsigned int cur_size, unsigned int need_size)
	{
		while(cur_size < need_size)
		{
			cur_size *= 2;
		}
		return cur_size;
	}

	void aio_lock(ASYNCIO *aio) ACQUIRE(aio->lock)
	{
		aio->lock.lock();
	}

	void aio_unlock(ASYNCIO *aio) RELEASE(aio->lock)
	{
		aio->lock.unlock();
		sphore_signal(&aio->sphore);
	}

	void aio_write_unlocked(ASYNCIO *aio, const void *buffer, unsigned size)
	{
		unsigned int remaining;
		remaining = aio->buffer_size - buffer_len(aio);

		// Don't allow full queue to distinguish between empty and full queue.
		if(size < remaining)
		{
			unsigned int remaining_contiguous = aio->buffer_size - aio->write_pos;
			if(size > remaining_contiguous)
			{
				mem_copy(aio->buffer + aio->write_pos, buffer, remaining_contiguous);
				size -= remaining_contiguous;
				buffer = ((unsigned char *)buffer) + remaining_contiguous;
				aio->write_pos = 0;
			}
			mem_copy(aio->buffer + aio->write_pos, buffer, size);
			aio->write_pos = (aio->write_pos + size) % aio->buffer_size;
		}
		else
		{
			// Add 1 so the new buffer isn't completely filled.
			unsigned int new_written = buffer_len(aio) + size + 1;
			unsigned int next_size = next_buffer_size(aio->buffer_size, new_written);
			unsigned int next_len = 0;
			unsigned char *next_buffer = (unsigned char *)malloc(next_size);

			struct BUFFERS buffers;
			buffer_ptrs(aio, &buffers);
			if(buffers.buf1)
			{
				mem_copy(next_buffer + next_len, buffers.buf1, buffers.len1);
				next_len += buffers.len1;
				if(buffers.buf2)
				{
					mem_copy(next_buffer + next_len, buffers.buf2, buffers.len2);
					next_len += buffers.len2;
				}
			}
			mem_copy(next_buffer + next_len, buffer, size);
			next_len += size;

			free(aio->buffer);
			aio->buffer = next_buffer;
			aio->buffer_size = next_size;
			aio->read_pos = 0;
			aio->write_pos = next_len;
		}
	}

	void aio_write(ASYNCIO *aio, const void *buffer, unsigned size)
	{
		aio_lock(aio);
		aio_write_unlocked(aio, buffer, size);
		aio_unlock(aio);
	}

	void aio_write_newline_unlocked(ASYNCIO *aio)
	{
#if defined(CONF_FAMILY_WINDOWS)
		aio_write_unlocked(aio, "\r\n", 2);
#else
		aio_write_unlocked(aio, "\n", 1);
#endif
	}

	void aio_write_newline(ASYNCIO *aio)
	{
		aio_lock(aio);
		aio_write_newline_unlocked(aio);
		aio_unlock(aio);
	}

	int aio_error(ASYNCIO *aio)
	{
		CLockScope ls(aio->lock);
		return aio->error;
	}

	void aio_close(ASYNCIO *aio)
	{
		{
			CLockScope ls(aio->lock);
			aio->finish = ASYNCIO_CLOSE;
		}
		sphore_signal(&aio->sphore);
	}

	void aio_wait(ASYNCIO *aio)
	{
		void *thread;
		{
			CLockScope ls(aio->lock);
			thread = aio->thread;
			aio->thread = nullptr;
			if(aio->finish == ASYNCIO_RUNNING)
			{
				aio->finish = ASYNCIO_EXIT;
			}
		}
		sphore_signal(&aio->sphore);
		thread_wait(thread);
	}

	void aio_free(ASYNCIO *aio)
	{
		aio->lock.lock();
		if(aio->thread)
		{
			thread_detach(aio->thread);
			aio->thread = nullptr;
		}
		aio_handle_free_and_unlock(aio);
	}

	struct THREAD_RUN
	{
		void (*threadfunc)(void *);
		void *u;
	};

#if defined(CONF_FAMILY_UNIX)
	static void *thread_run(void *user)
#elif defined(CONF_FAMILY_WINDOWS)
	static unsigned long __stdcall thread_run(void *user)
#else
#error not implemented
#endif
	{
#if defined(CONF_FAMILY_WINDOWS)
		CWindowsComLifecycle WindowsComLifecycle(false);
#endif
		struct THREAD_RUN *data = (THREAD_RUN *)user;
		void (*threadfunc)(void *) = data->threadfunc;
		void *u = data->u;
		free(data);
		threadfunc(u);
		return 0;
	}

	void *thread_init(void (*threadfunc)(void *), void *u, const char *name)
	{
		struct THREAD_RUN *data = (THREAD_RUN *)malloc(sizeof(*data));
		data->threadfunc = threadfunc;
		data->u = u;
#if defined(CONF_FAMILY_UNIX)
		{
			pthread_attr_t attr;
			dbg_assert(pthread_attr_init(&attr) == 0, "pthread_attr_init failure");
#if defined(CONF_PLATFORM_MACOS) && defined(__MAC_10_10) && __MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_10
			dbg_assert(pthread_attr_set_qos_class_np(&attr, QOS_CLASS_USER_INTERACTIVE, 0) == 0, "pthread_attr_set_qos_class_np failure");
#endif
			pthread_t id;
			dbg_assert(pthread_create(&id, &attr, thread_run, data) == 0, "pthread_create failure");
			return (void *)id;
		}
#elif defined(CONF_FAMILY_WINDOWS)
		HANDLE thread = CreateThread(nullptr, 0, thread_run, data, 0, nullptr);
		dbg_assert(thread != nullptr, "CreateThread failure");
		// TODO: Set thread name using SetThreadDescription (would require minimum Windows 10 version 1607)
		return thread;
#else
#error not implemented
#endif
	}

	void thread_wait(void *thread)
	{
#if defined(CONF_FAMILY_UNIX)
		dbg_assert(pthread_join((pthread_t)thread, nullptr) == 0, "pthread_join failure");
#elif defined(CONF_FAMILY_WINDOWS)
		dbg_assert(WaitForSingleObject((HANDLE)thread, INFINITE) == WAIT_OBJECT_0, "WaitForSingleObject failure");
		dbg_assert(CloseHandle(thread), "CloseHandle failure");
#else
#error not implemented
#endif
	}

	void thread_yield()
	{
#if defined(CONF_FAMILY_UNIX)
		dbg_assert(sched_yield() == 0, "sched_yield failure");
#elif defined(CONF_FAMILY_WINDOWS)
		Sleep(0);
#else
#error not implemented
#endif
	}

	void thread_detach(void *thread)
	{
#if defined(CONF_FAMILY_UNIX)
		dbg_assert(pthread_detach((pthread_t)thread) == 0, "pthread_detach failure");
#elif defined(CONF_FAMILY_WINDOWS)
		dbg_assert(CloseHandle(thread), "CloseHandle failure");
#else
#error not implemented
#endif
	}

	void thread_init_and_detach(void (*threadfunc)(void *), void *u, const char *name)
	{
		void *thread = thread_init(threadfunc, u, name);
		thread_detach(thread);
	}

#if defined(CONF_FAMILY_WINDOWS)
	void sphore_init(SEMAPHORE *sem)
	{
		*sem = CreateSemaphoreW(nullptr, 0, std::numeric_limits<LONG>::max(), nullptr);
		dbg_assert(*sem != nullptr, "CreateSemaphoreW failure");
	}
	void sphore_wait(SEMAPHORE *sem)
	{
		dbg_assert(WaitForSingleObject((HANDLE)*sem, INFINITE) == WAIT_OBJECT_0, "WaitForSingleObject failure");
	}
	void sphore_signal(SEMAPHORE *sem)
	{
		dbg_assert(ReleaseSemaphore((HANDLE)*sem, 1, nullptr), "ReleaseSemaphore failure");
	}
	void sphore_destroy(SEMAPHORE *sem)
	{
		dbg_assert(CloseHandle((HANDLE)*sem), "CloseHandle failure");
	}
#elif defined(CONF_PLATFORM_MACOS)
	void sphore_init(SEMAPHORE *sem)
	{
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "/%d.%p", pid(), (void *)sem);
		*sem = sem_open(aBuf, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, 0);
		dbg_assert(*sem != SEM_FAILED, "sem_open failure, errno=%d, name='%s'", errno, aBuf);
	}
	void sphore_wait(SEMAPHORE *sem)
	{
		while(true)
		{
			if(sem_wait(*sem) == 0)
				break;
			dbg_assert(errno == EINTR, "sem_wait failure");
		}
	}
	void sphore_signal(SEMAPHORE *sem)
	{
		dbg_assert(sem_post(*sem) == 0, "sem_post failure");
	}
	void sphore_destroy(SEMAPHORE *sem)
	{
		dbg_assert(sem_close(*sem) == 0, "sem_close failure");
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "/%d.%p", pid(), (void *)sem);
		dbg_assert(sem_unlink(aBuf) == 0, "sem_unlink failure");
	}
#elif defined(CONF_FAMILY_UNIX)
	void sphore_init(SEMAPHORE *sem)
	{
		dbg_assert(sem_init(sem, 0, 0) == 0, "sem_init failure");
	}
	void sphore_wait(SEMAPHORE *sem)
	{
		while(true)
		{
			if(sem_wait(sem) == 0)
				break;
			dbg_assert(errno == EINTR, "sem_wait failure");
		}
	}
	void sphore_signal(SEMAPHORE *sem)
	{
		dbg_assert(sem_post(sem) == 0, "sem_post failure");
	}
	void sphore_destroy(SEMAPHORE *sem)
	{
		dbg_assert(sem_destroy(sem) == 0, "sem_destroy failure");
	}
#endif

	struct SECURE_RANDOM_DATA
	{
		int initialized;
#if defined(CONF_FAMILY_WINDOWS)
		HCRYPTPROV provider;
#else
		IOHANDLE urandom;
#endif
	};

	static struct SECURE_RANDOM_DATA secure_random_data = {0};

	int secure_random_init()
	{
		if(secure_random_data.initialized)
		{
			return 0;
		}
#if defined(CONF_FAMILY_WINDOWS)
		if(CryptAcquireContext(&secure_random_data.provider, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		{
			secure_random_data.initialized = 1;
			return 0;
		}
		else
		{
			return 1;
		}
#else
		secure_random_data.urandom = io_open("/dev/urandom", IOFLAG_READ);
		if(secure_random_data.urandom)
		{
			secure_random_data.initialized = 1;
			return 0;
		}
		else
		{
			return 1;
		}
#endif
	}

	int secure_random_uninit()
	{
		if(!secure_random_data.initialized)
		{
			return 0;
		}
#if defined(CONF_FAMILY_WINDOWS)
		if(CryptReleaseContext(secure_random_data.provider, 0))
		{
			secure_random_data.initialized = 0;
			return 0;
		}
		else
		{
			return 1;
		}
#else
		if(!io_close(secure_random_data.urandom))
		{
			secure_random_data.initialized = 0;
			return 0;
		}
		else
		{
			return 1;
		}
#endif
	}

	void generate_password(char *buffer, unsigned length, const unsigned short *random, unsigned random_length)
	{
		static const char VALUES[] = "ABCDEFGHKLMNPRSTUVWXYZabcdefghjkmnopqt23456789";
		static const size_t NUM_VALUES = sizeof(VALUES) - 1; // Disregard the '\0'.
		unsigned i;
		dbg_assert(length >= random_length * 2 + 1, "too small buffer");
		dbg_assert(NUM_VALUES * NUM_VALUES >= 2048, "need at least 2048 possibilities for 2-character sequences");

		buffer[random_length * 2] = 0;

		for(i = 0; i < random_length; i++)
		{
			unsigned short random_number = random[i] % 2048;
			buffer[2 * i + 0] = VALUES[random_number / NUM_VALUES];
			buffer[2 * i + 1] = VALUES[random_number % NUM_VALUES];
		}
	}

#define MAX_PASSWORD_LENGTH 128

	void secure_random_password(char *buffer, unsigned length, unsigned pw_length)
	{
		unsigned short random[MAX_PASSWORD_LENGTH / 2];
		// With 6 characters, we get a password entropy of log(2048) * 6/2 = 33bit.
		dbg_assert(length >= pw_length + 1, "too small buffer");
		dbg_assert(pw_length >= 6, "too small password length");
		dbg_assert(pw_length % 2 == 0, "need an even password length");
		dbg_assert(pw_length <= MAX_PASSWORD_LENGTH, "too large password length");

		secure_random_fill(random, pw_length);

		generate_password(buffer, length, random, pw_length / 2);
	}

#undef MAX_PASSWORD_LENGTH

	void secure_random_fill(void *bytes, unsigned length)
	{
		if(!secure_random_data.initialized)
		{
			dbg_msg("secure", "called secure_random_fill before secure_random_init");
			dbg_break();
		}
#if defined(CONF_FAMILY_WINDOWS)
		if(!CryptGenRandom(secure_random_data.provider, length, (unsigned char *)bytes))
		{
			const DWORD LastError = GetLastError();
			const std::string ErrorMsg = windows_format_system_message(LastError);
			dbg_msg("secure", "CryptGenRandom failed: %ld %s", LastError, ErrorMsg.c_str());
			dbg_break();
		}
#else
		if(length != io_read(secure_random_data.urandom, bytes, length))
		{
			dbg_msg("secure", "io_read returned with a short read");
			dbg_break();
		}
#endif
	}

	int secure_rand()
	{
		unsigned int i;
		secure_random_fill(&i, sizeof(i));
		return (int)(i % RAND_MAX);
	}

	// From https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2.
	static unsigned int find_next_power_of_two_minus_one(unsigned int n)
	{
		n--;
		n |= n >> 1;
		n |= n >> 2;
		n |= n >> 4;
		n |= n >> 4;
		n |= n >> 16;
		return n;
	}

	int secure_rand_below(int below)
	{
		unsigned int mask = find_next_power_of_two_minus_one(below);
		dbg_assert(below > 0, "below must be positive");
		while(true)
		{
			unsigned int n;
			secure_random_fill(&n, sizeof(n));
			n &= mask;
			if((int)n < below)
			{
				return n;
			}
		}
	}

} // namespace polybob

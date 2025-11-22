#pragma once

#include "detect.h"
#include "types.h"

#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <chrono>
#include <cinttypes>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <functional>
#include <mutex>
#include <optional>
#include <string>

#ifdef __MINGW32__
#undef PRId64
#undef PRIu64
#undef PRIX64
#define PRId64 "I64d"
#define PRIu64 "I64u"
#define PRIX64 "I64X"
#define PRIzu "Iu"
#else
#define PRIzu "zu"
#endif

#ifdef CONF_FAMILY_UNIX
#include <sys/un.h>
#endif

#ifdef CONF_PLATFORM_LINUX
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#if __cplusplus >= 201703L
#define MAYBE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__)
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif

#ifdef __GNUC__
#define GNUC_ATTRIBUTE(x) __attribute__(x)
#else
#define GNUC_ATTRIBUTE(x)
#endif

#if defined(CONF_FAMILY_WINDOWS)
#elif defined(CONF_PLATFORM_MACOS)
#include <semaphore.h>
#elif defined(CONF_FAMILY_UNIX)
#include <semaphore.h>
#else
#error semaphore not supported on this platform
#endif

namespace polybob
{

#define dbg_assert(test, fmt, ...) \
	do \
	{ \
		if(!(test)) \
		{ \
			dbg_assert_imp(__FILE__, __LINE__, fmt, ##__VA_ARGS__); \
		} \
	} while(false)

#if defined(__cplusplus)
	[[noreturn]]
#endif
	void dbg_assert_imp(const char *filename, int line, const char *fmt, ...)
		GNUC_ATTRIBUTE((format(printf, 3, 4)));

#ifdef __clang_analyzer__
#include <cassert>
#undef dbg_assert
#define dbg_assert(test, fmt, ...) assert(test)
#endif

	bool dbg_assert_has_failed();

#if defined(__cplusplus)
	[[noreturn]]
#endif
	void dbg_break();

	typedef std::function<void(const char *message)> DBG_ASSERT_HANDLER;
	void dbg_assert_set_handler(DBG_ASSERT_HANDLER handler);

	void dbg_msg(const char *sys, const char *fmt, ...)
		GNUC_ATTRIBUTE((format(printf, 2, 3)));

	void mem_copy(void *dest, const void *source, size_t size);
	void mem_move(void *dest, const void *source, size_t size);
	template<typename T>
	inline void mem_zero(T *block, size_t size)
	{
		static_assert((std::is_trivially_constructible<T>::value && std::is_trivially_destructible<T>::value) || std::is_fundamental<T>::value);
		memset(block, 0, size);
	}
	int mem_comp(const void *a, const void *b, size_t size);
	bool mem_has_null(const void *block, size_t size);

	int str_toint(const char *str);
	bool str_toint(const char *str, int *out);
	int str_toint_base(const char *str, int base);
	unsigned long str_toulong_base(const char *str, int base);
	int64_t str_toint64_base(const char *str, int base = 10);
	float str_tofloat(const char *str);
	bool str_tofloat(const char *str, float *out);

	int str_length(const char *str);

	int str_format_v(char *buffer, int buffer_size, const char *format, va_list args)
		GNUC_ATTRIBUTE((format(printf, 3, 0)));
	int str_format(char *buffer, int buffer_size, const char *format, ...)
		GNUC_ATTRIBUTE((format(printf, 3, 4)));

#if !defined(CONF_DEBUG)
	int str_format_int(char *buffer, size_t buffer_size, int value);

	template<typename... Args>
	int str_format_opt(char *buffer, int buffer_size, const char *format, Args... args)
	{
		static_assert(sizeof...(args) > 0, "Use str_copy instead of str_format without format arguments");
		return str_format(buffer, buffer_size, format, args...);
	}

	template<>
	inline int str_format_opt(char *buffer, int buffer_size, const char *format, int val)
	{
		if(strcmp(format, "%d") == 0)
		{
			return str_format_int(buffer, buffer_size, val);
		}
		else
		{
			return str_format(buffer, buffer_size, format, val);
		}
	}

#define str_format str_format_opt
#endif

	void str_sanitize_cc(char *str);
	void str_sanitize(char *str);

	int str_isspace(char c);
	char str_uppercase(char c);
	bool str_isnum(char c);
	int str_isallnum(const char *str);
	int str_isallnum_hex(const char *str);

	int str_comp_nocase(const char *a, const char *b);
	int str_comp_nocase_num(const char *a, const char *b, int num);
	int str_comp(const char *a, const char *b);
	int str_comp_num(const char *a, const char *b, int num);
	int str_comp_filenames(const char *a, const char *b);
	const char *str_startswith_nocase(const char *str, const char *prefix);
	const char *str_startswith(const char *str, const char *prefix);
	const char *str_endswith_nocase(const char *str, const char *suffix);
	const char *str_endswith(const char *str, const char *suffix);
	const char *str_find_nocase(const char *haystack, const char *needle);
	const char *str_find(const char *haystack, const char *needle);

	void str_hex(char *dst, int dst_size, const void *data, int data_size);
	void str_hex_cstyle(char *dst, int dst_size, const void *data, int data_size, int bytes_per_line = 12);
	int str_hex_decode(void *dst, int dst_size, const char *src);
	void str_base64(char *dst, int dst_size, const void *data, int data_size);
	int str_base64_decode(void *dst, int dst_size, const char *data);

	void str_append(char *dst, const char *src, int dst_size);
	template<int N>
	void str_append(char (&dst)[N], const char *src)
	{
		str_append(dst, src, N);
	}
	int str_copy(char *dst, const char *src, int dst_size);
	template<int N>
	void str_copy(char (&dst)[N], const char *src)
	{
		str_copy(dst, src, N);
	}

	void str_timestamp(char *buffer, int buffer_size);
	void str_timestamp_format(char *buffer, int buffer_size, const char *format)
		GNUC_ATTRIBUTE((format(strftime, 3, 0)));
	void str_timestamp_ex(time_t time, char *buffer, int buffer_size, const char *format)
		GNUC_ATTRIBUTE((format(strftime, 4, 0)));
	bool timestamp_from_str(const char *string, const char *format, time_t *timestamp)
		GNUC_ATTRIBUTE((format(strftime, 2, 0)));

#define FORMAT_TIME "%H:%M:%S"
#define FORMAT_SPACE "%Y-%m-%d %H:%M:%S"
#define FORMAT_NOSPACE "%Y-%m-%d_%H-%M-%S"

	enum
	{
		TIME_DAYS,
		TIME_HOURS,
		TIME_MINS,
		TIME_HOURS_CENTISECS,
		TIME_MINS_CENTISECS,
		TIME_SECS_CENTISECS,
	};

	int str_time(int64_t centisecs, int format, char *buffer, int buffer_size);
	int str_time_float(float secs, int format, char *buffer, int buffer_size);
	void str_escape(char **dst, const char *src, const char *end);

	int str_utf8_tolower(int code);
	int str_utf8_comp_nocase(const char *a, const char *b);
	int str_utf8_comp_nocase_num(const char *a, const char *b, int num);
	const char *str_utf8_find_nocase(const char *haystack, const char *needle, const char **end = nullptr);
	int str_utf8_isspace(int code);
	int str_utf8_isstart(char c);
	const char *str_utf8_skip_whitespaces(const char *str);
	void str_utf8_trim_right(char *param);
	int str_utf8_rewind(const char *str, int cursor);
	int str_utf8_fix_truncation(char *str);
	int str_utf8_forward(const char *str, int cursor);
	int str_utf8_decode(const char **ptr);
	int str_utf8_encode(char *ptr, int chr);
	int str_utf8_check(const char *str);
	void str_utf8_copy_num(char *dst, const char *src, int dst_size, int num);
	void str_utf8_stats(const char *str, size_t max_size, size_t max_count, size_t *size, size_t *count);
	size_t str_utf8_offset_bytes_to_chars(const char *str, size_t byte_offset);
	size_t str_utf8_offset_chars_to_bytes(const char *str, size_t char_offset);

	enum
	{
		/**
		 * Open file for reading.
		 *
		 * @see io_open
		 */
		IOFLAG_READ = 1,
		/**
		 * Open file for writing.
		 *
		 * @see io_open
		 */
		IOFLAG_WRITE = 2,
		/**
		 * Open file for appending at the end.
		 *
		 * @see io_open
		 */
		IOFLAG_APPEND = 4,
	};

	enum ESeekOrigin
	{
		IOSEEK_START = 0,
		IOSEEK_CUR = 1,
		IOSEEK_END = 2,
	};

	IOHANDLE io_open(const char *filename, int flags);
	unsigned io_read(IOHANDLE io, void *buffer, unsigned size);
	bool io_read_all(IOHANDLE io, void **result, unsigned *result_len);
	char *io_read_all_str(IOHANDLE io);
	int io_skip(IOHANDLE io, int64_t size);
	int io_seek(IOHANDLE io, int64_t offset, ESeekOrigin origin);
	int64_t io_tell(IOHANDLE io);
	int64_t io_length(IOHANDLE io);
	unsigned io_write(IOHANDLE io, const void *buffer, unsigned size);
	bool io_write_newline(IOHANDLE io);
	int io_close(IOHANDLE io);
	int io_flush(IOHANDLE io);
	int io_sync(IOHANDLE io);
	int io_error(IOHANDLE io);
	IOHANDLE io_stdin();
	IOHANDLE io_stdout();
	IOHANDLE io_stderr();
	IOHANDLE io_current_exe();

	typedef struct ASYNCIO ASYNCIO;
	ASYNCIO *aio_new(IOHANDLE io);
	void aio_lock(ASYNCIO *aio);
	void aio_unlock(ASYNCIO *aio);
	void aio_write(ASYNCIO *aio, const void *buffer, unsigned size);
	void aio_write_newline(ASYNCIO *aio);
	void aio_write_unlocked(ASYNCIO *aio, const void *buffer, unsigned size);
	void aio_write_newline_unlocked(ASYNCIO *aio);
	int aio_error(ASYNCIO *aio);
	void aio_close(ASYNCIO *aio);
	void aio_wait(ASYNCIO *aio);
	void aio_free(ASYNCIO *aio);

	void *thread_init(void (*threadfunc)(void *), void *user, const char *name);
	void thread_wait(void *thread);
	void thread_yield();
	void thread_detach(void *thread);
	void thread_init_and_detach(void (*threadfunc)(void *), void *user, const char *name);

#if defined(CONF_FAMILY_WINDOWS)
	typedef void *SEMAPHORE;
#elif defined(CONF_PLATFORM_MACOS)
	typedef sem_t *SEMAPHORE;
#elif defined(CONF_FAMILY_UNIX)
	typedef sem_t SEMAPHORE;
#else
#error not implemented on this platform
#endif

	void sphore_init(SEMAPHORE *sem);
	void sphore_wait(SEMAPHORE *sem);
	void sphore_signal(SEMAPHORE *sem);
	void sphore_destroy(SEMAPHORE *sem);

	void set_new_tick();
	int64_t time_get_impl();
	int64_t time_get();
	int64_t time_freq();
	int64_t time_timestamp();
	int time_houroftheday();
	enum ETimeSeason
	{
		SEASON_SPRING = 0,
		SEASON_SUMMER,
		SEASON_AUTUMN,
		SEASON_WINTER,
		SEASON_EASTER,
		SEASON_HALLOWEEN,
		SEASON_XMAS,
		SEASON_NEWYEAR
	};
	ETimeSeason time_season();

	void generate_password(char *buffer, unsigned length, const unsigned short *random, unsigned random_length);
	[[nodiscard]] int secure_random_init();
	int secure_random_uninit();
	void secure_random_password(char *buffer, unsigned length, unsigned pw_length);
	void secure_random_fill(void *bytes, unsigned length);
	int secure_rand();
	int secure_rand_below(int below);

} // namespace polybob

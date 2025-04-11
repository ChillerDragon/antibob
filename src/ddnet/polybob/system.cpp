#include <polybob/system.h>

#include <chrono>

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

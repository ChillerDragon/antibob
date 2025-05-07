#include <polybob/base/system.h>
#include <polybob/base/system/str.h>

#include <string.h>
#include <zlib.h>

namespace polybob {

void str_truncate(char *dst, int dst_size, const char *src, int truncation_len)
{
	int size = dst_size;
	if(truncation_len < size)
	{
		size = truncation_len + 1;
	}
	str_copy(dst, src, size);
}

const char *str_rchr(const char *haystack, char needle)
{
	return strrchr(haystack, needle);
}

} // namespace polybob

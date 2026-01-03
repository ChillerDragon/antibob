#include <polybob/base/system.h>
#include <polybob/base/system/str.h>
#include <string.h>
#include <zlib.h>

namespace polybob
{

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

	static const char *str_token_get(const char *str, const char *delim, int *length)
	{
		size_t len = strspn(str, delim);
		if(len > 1)
			str++;
		else
			str += len;
		if(!*str)
			return nullptr;

		*length = strcspn(str, delim);
		return str;
	}

	const char *str_next_token(const char *str, const char *delim, char *buffer, int buffer_size)
	{
		int len = 0;
		const char *tok = str_token_get(str, delim, &len);
		if(len < 0 || tok == nullptr)
		{
			buffer[0] = '\0';
			return nullptr;
		}

		len = buffer_size > len ? len : buffer_size - 1;
		mem_copy(buffer, tok, len);
		buffer[len] = '\0';

		return tok + len;
	}

} // namespace polybob

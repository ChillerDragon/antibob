#pragma once

namespace polybob
{

	void str_truncate(char *dst, int dst_size, const char *src, int truncation_len);
	const char *str_rchr(const char *haystack, char needle);
	const char *str_next_token(const char *str, const char *delim, char *buffer, int buffer_size);

} // namespace polybob

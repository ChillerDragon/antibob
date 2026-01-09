#pragma once

namespace polybob
{

	void str_truncate(char *dst, int dst_size, const char *src, int truncation_len);
	const char *str_rchr(const char *haystack, char needle);
	const char *str_next_token(const char *str, const char *delim, char *buffer, int buffer_size);
	int str_isspace(char c);
	char *str_skip_to_whitespace(char *str);
	const char *str_skip_to_whitespace_const(const char *str);
	char *str_skip_whitespaces(char *str);
	const char *str_skip_whitespaces_const(const char *str);

} // namespace polybob

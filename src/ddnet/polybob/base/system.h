#pragma once

#include <cstdint>
#include <cstdlib>

int str_toint(const char *str);
bool str_toint(const char *str, int *out);
int str_toint_base(const char *str, int base);
unsigned long str_toulong_base(const char *str, int base);
int64_t str_toint64_base(const char *str, int base = 10);
float str_tofloat(const char *str);
bool str_tofloat(const char *str, float *out);

int str_length(const char *str);

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

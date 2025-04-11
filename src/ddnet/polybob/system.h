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

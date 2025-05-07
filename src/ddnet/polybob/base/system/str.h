#pragma once

namespace polybob {

void str_truncate(char *dst, int dst_size, const char *src, int truncation_len);
const char *str_rchr(const char *haystack, char needle);

} // namespace polybob

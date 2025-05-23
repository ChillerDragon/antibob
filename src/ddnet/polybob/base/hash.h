#pragma once

#include <cstddef> // size_t

namespace polybob {

enum
{
	SHA256_DIGEST_LENGTH = 256 / 8,
	SHA256_MAXSTRSIZE = 2 * SHA256_DIGEST_LENGTH + 1,
	MD5_DIGEST_LENGTH = 128 / 8,
	MD5_MAXSTRSIZE = 2 * MD5_DIGEST_LENGTH + 1,
};

struct SHA256_DIGEST
{
	unsigned char data[SHA256_DIGEST_LENGTH];
};

struct MD5_DIGEST
{
	unsigned char data[MD5_DIGEST_LENGTH];
};

SHA256_DIGEST sha256(const void *message, size_t message_len);
void sha256_str(SHA256_DIGEST digest, char *str, size_t max_len);
int sha256_from_str(SHA256_DIGEST *out, const char *str);
int sha256_comp(SHA256_DIGEST digest1, SHA256_DIGEST digest2);

MD5_DIGEST md5(const void *message, size_t message_len);
void md5_str(MD5_DIGEST digest, char *str, size_t max_len);
int md5_from_str(MD5_DIGEST *out, const char *str);
int md5_comp(MD5_DIGEST digest1, MD5_DIGEST digest2);

extern const SHA256_DIGEST SHA256_ZEROED;

inline bool operator==(const SHA256_DIGEST &that, const SHA256_DIGEST &other)
{
	return sha256_comp(that, other) == 0;
}
inline bool operator!=(const SHA256_DIGEST &that, const SHA256_DIGEST &other)
{
	return !(that == other);
}
inline bool operator==(const MD5_DIGEST &that, const MD5_DIGEST &other)
{
	return md5_comp(that, other) == 0;
}
inline bool operator!=(const MD5_DIGEST &that, const MD5_DIGEST &other)
{
	return !(that == other);
}

} // namespace polybob

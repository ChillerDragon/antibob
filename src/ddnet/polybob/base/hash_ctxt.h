#pragma once

#include "hash.h"
#include <cstdint>

#include <polybob/engine/external/md5/md5.h>

namespace polybob {

struct SHA256_CTX
{
	uint64_t length;
	uint32_t state[8];
	uint32_t curlen;
	unsigned char buf[64];
};
typedef md5_state_t MD5_CTX;

void sha256_init(SHA256_CTX *ctxt);
void sha256_update(SHA256_CTX *ctxt, const void *data, size_t data_len);
SHA256_DIGEST sha256_finish(SHA256_CTX *ctxt);

void md5_init(MD5_CTX *ctxt);
void md5_update(MD5_CTX *ctxt, const void *data, size_t data_len);
MD5_DIGEST md5_finish(MD5_CTX *ctxt);

} // namespace polybob

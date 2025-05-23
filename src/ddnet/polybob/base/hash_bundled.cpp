#include "hash_ctxt.h"

#include <polybob/engine/external/md5/md5.h>

namespace polybob {

void md5_update(MD5_CTX *ctxt, const void *data, size_t data_len)
{
	md5_append(ctxt, (const md5_byte_t *)data, data_len);
}

MD5_DIGEST md5_finish(MD5_CTX *ctxt)
{
	MD5_DIGEST result;
	md5_finish_(ctxt, result.data);
	return result;
}

} // namespace polybob

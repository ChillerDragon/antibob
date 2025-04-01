#pragma once

enum
{
	__MAGIC_OFFSET_TO_NOT_CONFLICT_WITH_DDNET = 90000,
#define UUID(id, name) id,
#include <bob/protocol_ex_msgs.h>
#undef UUID
};

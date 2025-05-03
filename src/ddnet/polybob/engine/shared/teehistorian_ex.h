#pragma once

#include "protocol_ex.h"

namespace polybob {

enum
{
	__TEEHISTORIAN_UUID_HELPER = OFFSET_TEEHISTORIAN_UUID - 1,
#define UUID(id, name) id,
#include "teehistorian_ex_chunks.h"
#undef UUID
	OFFSET_GAME_UUID
};

void RegisterTeehistorianUuids(class CBobUuidManager *pManager);

} // namespace polybob

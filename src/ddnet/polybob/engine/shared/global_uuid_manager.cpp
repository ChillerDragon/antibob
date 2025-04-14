#include "protocol_ex.h"
#include "teehistorian_ex.h"
#include "uuid_manager.h"

#include <polybob/engine/uuid.h>

static CUuidManager CreateGlobalUuidManager()
{
	CUuidManager Manager;
	RegisterUuids(&Manager);
	RegisterTeehistorianUuids(&Manager);
	RegisterGameUuids(&Manager);
	return Manager;
}

CUuidManager g_UuidManager = CreateGlobalUuidManager();

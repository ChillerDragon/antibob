#include "bob_uuid_manager.h"
#include "protocol_ex.h"
#include "teehistorian_ex.h"

#include <polybob/engine/uuid.h>

static CBobUuidManager CreateGlobalBobUuidManager()
{
	CBobUuidManager Manager;
	RegisterUuids(&Manager);
	RegisterTeehistorianUuids(&Manager);
	polybob::RegisterGameUuids(&Manager);
	return Manager;
}

CBobUuidManager g_BobUuidManager = CreateGlobalBobUuidManager();

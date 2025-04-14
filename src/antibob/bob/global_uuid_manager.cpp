#include <bob/protocol_ex.h>
#include <bob/uuid_manager.h>

#include <polybob/engine/uuid.h>

static CBobUuidManager CreateGlobalBobUuidManager()
{
	CBobUuidManager Manager;
	BobRegisterUuids(&Manager);
	return Manager;
}

CBobUuidManager g_BobUuidManager = CreateGlobalBobUuidManager();

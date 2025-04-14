#include "teehistorian_ex.h"

#include <polybob/engine/shared/bob_uuid_manager.h>

void RegisterTeehistorianUuids(CBobUuidManager *pManager)
{
#define UUID(id, name) pManager->RegisterName(id, name);
#include "teehistorian_ex_chunks.h"
#undef UUID
}

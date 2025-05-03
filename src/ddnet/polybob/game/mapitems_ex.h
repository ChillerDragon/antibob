#include <polybob/game/generated/protocol.h>

namespace polybob {

enum
{
	__MAPITEMTYPE_UUID_HELPER = polybob::OFFSET_MAPITEMTYPE_UUID - 1,
#define UUID(id, name) id,
#include "mapitems_ex_types.h"
#undef UUID
	END_MAPITEMTYPES_UUID,
};

void RegisterMapItemTypeUuids(class CBobUuidManager *pManager);

} // namespace polybob

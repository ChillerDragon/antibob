#pragma once

#include <polybob/engine/shared/bob_uuid_manager.h>

namespace polybob {

class CMsgPacker;
class CUnpacker;

enum
{
	NETMSG_EX_INVALID = UUID_INVALID,
	NETMSG_EX_UNKNOWN = UUID_UNKNOWN,

	OFFSET_NETMSG_UUID = OFFSET_UUID,

	__NETMSG_UUID_HELPER = OFFSET_NETMSG_UUID - 1,
#define UUID(id, name) id,
#include "protocol_ex_msgs.h"
#undef UUID
	OFFSET_TEEHISTORIAN_UUID,

	UNPACKMESSAGE_ERROR = 0,
	UNPACKMESSAGE_OK,
	UNPACKMESSAGE_ANSWER,

	SERVERCAP_CURVERSION = 5,
	SERVERCAPFLAG_DDNET = 1 << 0,
	SERVERCAPFLAG_CHATTIMEOUTCODE = 1 << 1,
	SERVERCAPFLAG_ANYPLAYERFLAG = 1 << 2,
	SERVERCAPFLAG_PINGEX = 1 << 3,
	SERVERCAPFLAG_ALLOWDUMMY = 1 << 4,
	SERVERCAPFLAG_SYNCWEAPONINPUT = 1 << 5,
};

void RegisterUuids(CBobUuidManager *pManager);
int BobUnpackMessageId(int *pId, bool *pSys, CUuid *pUuid, polybob::CUnpacker *pUnpacker, CMsgPacker *pPacker);

} // namespace polybob

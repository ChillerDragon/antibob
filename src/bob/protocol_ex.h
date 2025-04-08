#pragma once

#include <game/mapitems_ex.h>

#include <bob/uuid_manager.h>

class CMsgPacker;
class CUnpacker;

enum
{
	__NETMSG_BOB_UUID_HELPER = OFFSET_NETMSG_UUID - 1,
#define UUID(id, name) id,
#include <bob/protocol_ex_msgs.h>
#undef UUID
};

#define RegisterUuids_USE_BobRegisterUuids_INSTEAD

// reimplementation of ddnet's RegisterUuids
// to be independent in the antibot module
void BobRegisterUuids(CBobUuidManager *pManager);


#define UnpackMessageId USE_BobUnpackMessageId_INSTEAD

// reimplementation of ddnet's UnpackMessageId
// to be independent in the antibot module
int BobUnpackMessageId(int *pId, bool *pSys, CUuid *pUuid, CUnpacker *pUnpacker, CMsgPacker *pPacker);

#pragma once

// included for CUuid, CName and CNameIndexed
// it sadly exposes the g_UuidManager which should NOT be used
// the g_BobUuidManager should be used instead
#include <polybob/engine/shared/uuid_manager.h>

#define g_UuidManager DO_NOT_USE_g_UuidManager_USE_g_BobUuidManager_INSTEAD

#include <vector>

// reimplementation of ddnet's CUuidManager
// to be independent in the antibot module
class CBobUuidManager
{
	std::vector<CName> m_vNames;
	std::vector<CNameIndexed> m_vNamesSorted;

public:
	void RegisterName(int Id, const char *pName);
	CUuid GetUuid(int Id) const;
	const char *GetName(int Id) const;
	int LookupUuid(CUuid Uuid) const;
	int NumUuids() const;

	int UnpackUuid(CUnpacker *pUnpacker) const;
	int UnpackUuid(CUnpacker *pUnpacker, CUuid *pOut) const;
	void PackUuid(int Id, CPacker *pPacker) const;

	void DebugDump() const;
};

extern CBobUuidManager g_BobUuidManager;

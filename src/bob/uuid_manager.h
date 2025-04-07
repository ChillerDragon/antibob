#pragma once

#include <engine/shared/uuid_manager.h>

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

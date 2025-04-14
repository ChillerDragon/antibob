#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/engine/shared/packer.h>
#include <polybob/engine/shared/uuid_manager.h>

#include <algorithm>
#include <cstdio>

#include "uuid_manager.h"

static int GetIndex(int Id)
{
	return Id - OFFSET_UUID;
}

static int GetId(int Index)
{
	return Index + OFFSET_UUID;
}

void CBobUuidManager::RegisterName(int Id, const char *pName)
{
	dbg_assert(GetIndex(Id) == (int)m_vNames.size(), "antibob uuid names must be registered with increasing ID (got=%d want=%d)", GetIndex(Id), (int)m_vNames.size());
	CName Name;
	Name.m_pName = pName;
	Name.m_Uuid = CalculateUuid(pName);
	dbg_assert(LookupUuid(Name.m_Uuid) == -1, "duplicate uuid");

	m_vNames.push_back(Name);

	CNameIndexed NameIndexed;
	NameIndexed.m_Uuid = Name.m_Uuid;
	NameIndexed.m_Id = GetIndex(Id);
	m_vNamesSorted.insert(std::lower_bound(m_vNamesSorted.begin(), m_vNamesSorted.end(), NameIndexed), NameIndexed);
}

CUuid CBobUuidManager::GetUuid(int Id) const
{
	return m_vNames[GetIndex(Id)].m_Uuid;
}

const char *CBobUuidManager::GetName(int Id) const
{
	return m_vNames[GetIndex(Id)].m_pName;
}

int CBobUuidManager::LookupUuid(CUuid Uuid) const
{
	CNameIndexed Needle;
	Needle.m_Uuid = Uuid;
	Needle.m_Id = 0;
	auto Range = std::equal_range(m_vNamesSorted.begin(), m_vNamesSorted.end(), Needle);
	if(std::distance(Range.first, Range.second) == 1)
	{
		return GetId(Range.first->m_Id);
	}
	return UUID_UNKNOWN;
}

int CBobUuidManager::NumUuids() const
{
	return m_vNames.size();
}

int CBobUuidManager::UnpackUuid(CUnpacker *pUnpacker) const
{
	CUuid Temp;
	return UnpackUuid(pUnpacker, &Temp);
}

int CBobUuidManager::UnpackUuid(CUnpacker *pUnpacker, CUuid *pOut) const
{
	const CUuid *pUuid = (const CUuid *)pUnpacker->GetRaw(sizeof(*pUuid));
	if(pUuid == nullptr)
	{
		return UUID_INVALID;
	}
	*pOut = *pUuid;
	return LookupUuid(*pUuid);
}

void CBobUuidManager::PackUuid(int Id, CPacker *pPacker) const
{
	CUuid Uuid = GetUuid(Id);
	pPacker->AddRaw(&Uuid, sizeof(Uuid));
}

void CBobUuidManager::DebugDump() const
{
	for(const auto &Name : m_vNames)
	{
		char aBuf[UUID_MAXSTRSIZE];
		FormatUuid(Name.m_Uuid, aBuf, sizeof(aBuf));
		log_debug("antibot", "uuid %s %s", aBuf, Name.m_pName);
	}
}

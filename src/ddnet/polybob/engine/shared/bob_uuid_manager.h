#pragma once

#include <vector>

enum
{
	UUID_MAXSTRSIZE = 37, // 12345678-0123-5678-0123-567890123456

	UUID_INVALID = -2,
	UUID_UNKNOWN = -1,

	OFFSET_UUID = 1 << 16,
};

struct CUuid
{
	unsigned char m_aData[16];

	bool operator==(const CUuid &Other) const;
	bool operator!=(const CUuid &Other) const;
	bool operator<(const CUuid &Other) const;
};

extern const CUuid BOB_UUID_ZEROED;

CUuid RandomUuid();
CUuid CalculateUuid(const char *pName);
// The buffer length should be at least UUID_MAXSTRSIZE.
void FormatUuid(CUuid Uuid, char *pBuffer, unsigned BufferLength);
// Returns nonzero on failure.
int ParseUuid(CUuid *pUuid, const char *pBuffer);

struct CName
{
	CUuid m_Uuid;
	const char *m_pName;
};

struct CNameIndexed
{
	CUuid m_Uuid;
	int m_Id;

	bool operator<(const CNameIndexed &Other) const { return m_Uuid < Other.m_Uuid; }
	bool operator==(const CNameIndexed &Other) const { return m_Uuid == Other.m_Uuid; }
};

namespace polybob {
class CPacker;
class CUnpacker;
}

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

	int UnpackUuid(polybob::CUnpacker *pUnpacker) const;
	int UnpackUuid(polybob::CUnpacker *pUnpacker, CUuid *pOut) const;
	void PackUuid(int Id, polybob::CPacker *pPacker) const;

	void DebugDump() const;
};

extern CBobUuidManager g_BobUuidManager;

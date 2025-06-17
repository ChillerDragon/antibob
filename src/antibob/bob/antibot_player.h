#pragma once

#include <polybob/base/lock.h>
#include <polybob/base/types.h>
#include <polybob/engine/shared/http.h>
#include <polybob/engine/shared/jobs.h>
#include <polybob/engine/shared/protocol.h>
#include <polybob/game/generated/protocol.h>

#include <bob/detection_event.h>

#include <cstdint>
#include <memory>
#include <unordered_map>

class CAbstractLookupPlayerJob : public polybob::IJob
{
public:
	CAbstractLookupPlayerJob(class CAntibob *pAntibob, int ClientId, const char *pName, const char *pAddr);

	// output
	bool m_KnownCheater = false;

protected:
	// input
	class CAntibob *m_pAntibob;
	int m_ClientId;
	char m_aName[polybob::MAX_NAME_LENGTH];
	char m_aAddr[128];
};

class CLookupPlayerJob : public CAbstractLookupPlayerJob
{
public:
	CLookupPlayerJob(class CAntibob *pAntibob, int ClientId, const char *pName, const char *pAddr);

protected:
	void Run() override REQUIRES(!m_Lock);

private:
	polybob::CLock m_Lock;
	std::shared_ptr<polybob::CHttpRequest> m_pGetRequest GUARDED_BY(m_Lock);
};

class CAntibotPlayer
{
public:
	CAntibotPlayer(int ClientId, uint32_t UniqueClientId, bool Sixup, const char *pAddr);

	// starting 1 to make 0 the special value "no client id"
	uint32_t m_UniqueClientId = 0;
	int m_ClientId;
	bool m_Sixup;
	polybob::NETADDR m_Addr;

	bool m_KnownCheater = false;
	// TODO: for now the job is only launched on join
	//       ideally it would happen on name change too
	//       and some results should be cached
	//       to avoid spamming http requests
	std::shared_ptr<CAbstractLookupPlayerJob> m_pLookupJob = nullptr;
	polybob::CNetObj_PlayerInput m_aInputs[10];
	int m_SentInputs = 0;
	bool InputHistoryValid() const { return m_SentInputs > std::size(m_aInputs); }
	void DumpInputHistory();
	void OnTick();

	// same as ddnet's CPlayer::m_Ready
	// indicating if the client info was already sent
	bool m_Ready = false;

	std::unordered_map<int, CDetectionEvent> m_DetectionEvents;

	// registers a detection event
	// that will show up in the "dump_antibot" rcon command
	void Detect(int EventId, const char *pInfo = nullptr);

	//
	// Add new member variables for forks below this comment to avoid merge conflicts.
	//
};

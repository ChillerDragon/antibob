#include "antibot_player.h"

#include <bob/antibob.h>
#include <polybob/base/log.h>
#include <polybob/base/system/net.h>
#include <polybob/engine/shared/http.h>
#include <polybob/engine/shared/jobs.h>

#include <cstdint>
#include <memory>

using namespace polybob;

CAbstractLookupPlayerJob::CAbstractLookupPlayerJob(CAntibob *pAntibob, int ClientId, const char *pName, const char *pAddr, const char *pApiUrl, const char *pApiToken)
{
	m_pAntibob = pAntibob;
	m_ClientId = ClientId;
	str_copy(m_aName, pName);
	str_copy(m_aAddr, pAddr);
	str_copy(m_aApiUrl, pApiUrl);
	str_copy(m_aApiToken, pApiToken);
}

CLookupPlayerJob::CLookupPlayerJob(CAntibob *pAntibob, int ClientId, const char *pName, const char *pAddr, const char *pApiUrl, const char *pApiToken) :
	CAbstractLookupPlayerJob(pAntibob, ClientId, pName, pAddr, pApiUrl, pApiToken)
{
}

void CLookupPlayerJob::Run()
{
	char aUrl[2048];
	char aName[512];
	char aAddr[512];
	EscapeUrl(aName, m_aName);
	EscapeUrl(aAddr, m_aAddr);
	str_format(aUrl, sizeof(aUrl), "%s/player?name=%s&addr=%s", m_aApiUrl, aName, aAddr);

	char aAuthHeader[1024];
	str_format(aAuthHeader, sizeof(aAuthHeader), "Authorization: Bearer %s", m_aApiToken);
	const CTimeout Timeout{10000, 0, 8192, 10};
	const size_t MaxResponseSize = 10 * 1024 * 1024; // 10 MiB

	std::shared_ptr<CHttpRequest> pGet = HttpGet(aUrl);
	pGet->Header(aAuthHeader);
	pGet->Timeout(Timeout);
	pGet->MaxResponseSize(MaxResponseSize);
	pGet->ValidateBeforeOverwrite(true);
	pGet->LogProgress(HTTPLOG::NONE);
	pGet->FailOnErrorStatus(false);
	{
		const CLockScope LockScope(m_Lock);
		m_pGetRequest = pGet;
	}
	m_pAntibob->Http()->Run(pGet);
	pGet->Wait();
	{
		const CLockScope LockScope(m_Lock);
		m_pGetRequest = nullptr;
	}
	if(pGet->State() != EHttpState::DONE || State() == IJob::STATE_ABORTED || pGet->StatusCode() == 404)
	{
		return;
	}

	const json_value *pJson = pGet->ResultJson();
	if(!pJson)
	{
		log_error("antibot", "got no json from %s", aUrl);
		return;
	}

	m_KnownCheater = true;

	// TODO: somehow that did not work
	// const json_value &Json = *pJson;
	// const json_value &Names = Json["names"];
}

CAntibotPlayer::CAntibotPlayer(int ClientId, uint32_t UniqueClientId, int64_t JoinTick, bool Sixup, const char *pAddr) :
	m_ClientId(ClientId), m_UniqueClientId(UniqueClientId), m_JoinTick(JoinTick), m_Sixup(Sixup)
{
	net_addr_from_str(&m_Addr, pAddr);
	for(CNetObj_PlayerInput &Input : m_aInputs)
		Input = {};
}

void CAntibotPlayer::DumpInputHistory()
{
	log_info("antibot", "input history for cid=%d", m_ClientId);
	int i = 0;
	for(polybob::CNetObj_PlayerInput &Input : m_aInputs)
	{
		if(i++ >= m_SentInputs)
			break;

		log_info(
			"antibot",
			" aim=%d/%d dir=%d fire=%d jump=%d hook=%d",
			Input.m_TargetX,
			Input.m_TargetY,
			Input.m_Direction,
			Input.m_Fire,
			Input.m_Jump,
			Input.m_Hook);
	}
}

void CAntibotPlayer::Detect(int EventId, const char *pInfo, int Confidence)
{
	CDetectionEvent::PushAndStack(m_DetectionEvents, CDetectionEvent(EventId, pInfo, Confidence));
}

void CAntibotPlayer::OnTick()
{
}

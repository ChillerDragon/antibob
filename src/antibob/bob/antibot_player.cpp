#include <polybob/base/log.h>

#include <cstdint>

#include "antibot_player.h"

CAntibotPlayer::CAntibotPlayer(int ClientId, uint32_t UniqueClientId, bool Sixup) :
	m_ClientId(ClientId), m_UniqueClientId(UniqueClientId), m_Sixup(Sixup)
{
}

void CAntibotPlayer::DumpAimHistory()
{
	log_info("antibot", "input history for cid=%d", m_ClientId);
	for(polybob::CNetObj_PlayerInput &Input : m_aInputs)
	{
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

void CAntibotPlayer::Detect(int EventId, const char *pInfo)
{
	CDetectionEvent::PushAndStack(m_DetectionEvents, CDetectionEvent(EventId, pInfo));
}

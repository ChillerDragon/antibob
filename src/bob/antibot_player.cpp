#include "antibot_player.h"

CAntibotPlayer::CAntibotPlayer(int ClientId, bool Sixup) :
	m_ClientId(ClientId), m_Sixup(Sixup)
{
}

void CAntibotPlayer::Detect(int EventId, const char *pInfo)
{
	CDetectionEvent::PushAndStack(m_DetectionEvents, CDetectionEvent(EventId, pInfo));
}

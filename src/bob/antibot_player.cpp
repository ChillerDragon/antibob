#include "antibot_player.h"

CAntibotPlayer::CAntibotPlayer(int ClientId, bool Sixup) :
	m_ClientId(ClientId), m_Sixup(Sixup)
{
}

void CAntibotPlayer::Detect(int EventId)
{
	CDetectionEvent::PushAndStack(m_DetectionEvents, CDetectionEvent(EventId));
}

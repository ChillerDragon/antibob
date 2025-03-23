#include <engine/shared/protocol.h>
#include <game/generated/protocol.h>

#include "base.h"
#include "network.h"

CBase::CBase(CAntibotData *pData) :
	m_pData(pData)
{
	m_Network.OnInit(pData);
}

void CBase::SendChatTarget(int ClientId, const char *pMessage)
{
	CNetMsg_Sv_Chat Msg;
	Msg.m_Team = 0;
	Msg.m_ClientId = -1;
	Msg.m_pMessage = pMessage;
	Server()->SendPackMsg(&Msg, MSGFLAG_VITAL, ClientId);
}

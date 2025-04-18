#include <polybob/engine/shared/protocol.h>
#include <polybob/game/generated/protocol.h>

#include <bob/network.h>

#include "gameserver.h"

CGameServer::CGameServer(CAntibotData *pData) :
	m_pData(pData)
{
	m_Network.OnInit(pData);
}

void CGameServer::SendChat(int ClientId, int Team, const char *pMessage)
{
	CNetMsg_Sv_Chat Msg;
	Msg.m_Team = Team;
	Msg.m_ClientId = ClientId;
	Msg.m_pMessage = pMessage;
	Server()->SendPackMsg(&Msg, MSGFLAG_VITAL, -1);
}

void CGameServer::SendChatTarget(int ClientId, const char *pMessage)
{
	CNetMsg_Sv_Chat Msg;
	Msg.m_Team = 0;
	Msg.m_ClientId = -1;
	Msg.m_pMessage = pMessage;
	Server()->SendPackMsg(&Msg, MSGFLAG_VITAL, ClientId);
}

//
// antibot callbacks
//

void CGameServer::Kick(int ClientId, const char *pReason) const
{
	if(!pReason || pReason[0] == '\0')
		pReason = Config()->m_AbKickReason;
	m_pData->m_pfnKick(ClientId, pReason, m_pData->m_pUser);
}

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/base/system/shell.h>
#include <polybob/engine/shared/protocol.h>
#include <polybob/engine/storage.h>
#include <polybob/game/generated/protocol.h>

#include <bob/cmdline_arguments.h>
#include <bob/network.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gameserver.h"

CGameServer::CGameServer(CAntibotData *pData) :
	m_pData(pData)
{
	m_BobAbi.OnInit();
	m_Network.OnInit(pData, this);

	CCmdlineArguments CliArgs;
	m_pStorage = polybob::CreateStorage(IStorage::EInitializationType::SERVER, 1, (const char **)CliArgs.All());
}

CGameServer::~CGameServer()
{
	log_info("antibot", "shutting down antibob ...");
	delete m_pStorage;
}

void CGameServer::SendChat(int ClientId, int Team, const char *pMessage)
{
	polybob::CNetMsg_Sv_Chat Msg;
	Msg.m_Team = Team;
	Msg.m_ClientId = ClientId;
	Msg.m_pMessage = pMessage;
	Server()->SendPackMsg(&Msg, polybob::MSGFLAG_VITAL, -1);
}

void CGameServer::SendChatTarget(int ClientId, const char *pMessage)
{
	polybob::CNetMsg_Sv_Chat Msg;
	Msg.m_Team = 0;
	Msg.m_ClientId = -1;
	Msg.m_pMessage = pMessage;
	Server()->SendPackMsg(&Msg, polybob::MSGFLAG_VITAL, ClientId);
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

void CGameServer::LogInfo(const char *pFormat, ...)
{
	va_list Args;
	va_start(Args, pFormat);
	char aBuf[4000];
	str_format_v(aBuf, sizeof(aBuf), pFormat, Args);
	va_end(Args);

	m_pData->m_pfnLog(aBuf, m_pData->m_pUser);
}

void CGameServer::LogError(const char *pFormat, ...)
{
	va_list Args;
	va_start(Args, pFormat);
	char aBuf[4000];
	str_format_v(aBuf, sizeof(aBuf), pFormat, Args);
	va_end(Args);

	m_pData->m_pfnLog(aBuf, m_pData->m_pUser);
}

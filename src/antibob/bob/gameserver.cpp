#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/base/system/shell.h>
#include <polybob/engine/shared/protocol.h>
#include <polybob/engine/storage.h>
#include <polybob/game/generated/protocol.h>

#include <bob/cmdline_arguments.h>
#include <bob/network.h>
#include <bob/pending_punish.h>

#include <cstdarg>
#include <stdlib.h>
#include <thread>
#include <unistd.h>

#include "gameserver.h"

CGameServer::CGameServer(CAntibotData *pData) :
	m_pData(pData)
{
	m_BobAbi.OnInit();
	m_Network.OnInit(pData, this);

	CCmdlineArguments CliArgs;
	m_pStorage = polybob::CreateStorage(IStorage::EInitializationType::SERVER, 1, (const char **)CliArgs.All());
	m_JobPool.Init(std::max(4, (int)std::thread::hardware_concurrency()) - 2);
}

CGameServer::~CGameServer()
{
	log_info("antibot", "shutting down antibob ...");
	delete m_pStorage;
	log_info("antibot", "shutting down job pool ...");
	m_JobPool.Shutdown();
}

CAntibotPlayer *CGameServer::GetPlayerByUniqueClientId(int UniqueClientId)
{
	for(CAntibotPlayer *pPlayer : m_apPlayers)
	{
		if(!pPlayer)
			continue;
		if(pPlayer->m_UniqueClientId != UniqueClientId)
			continue;

		return pPlayer;
	}
	return nullptr;
}

bool CGameServer::IsClientReady(int ClientId) const
{
	if(!m_apPlayers[ClientId])
		return false;
	return m_apPlayers[ClientId]->m_Ready;
}

void CGameServer::AddJob(std::shared_ptr<polybob::IJob> pJob)
{
	m_JobPool.Add(std::move(pJob));
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

void CGameServer::Punish(int ClientId, const char *pReason, int TimeInMinutes, CPendingPunish::EPunish Punish)
{
	m_PunishController.SchedulePunish(ClientId, pReason, TimeInMinutes, Punish);
}

void CGameServer::Detect(int ClientId, int EventId, const char *pInfo)
{
	m_apPlayers[ClientId]->Detect(EventId, pInfo);
	if(Config()->m_AbLogEvents)
		LogEvent(ClientId, EventId, pInfo);
}

void CGameServer::LogEvent(int ClientId, int EventId, const char *pInfo)
{
	// TODO: this filename will conflict when multiple servers try to write to it
	//       ideally the filename would include the port of the server
	//       or it should write to a properly locked sqlite3 database instead

	const char *pFilename = "antibob_events.txt";
	IOHANDLE File = Storage()->OpenFile(pFilename, IOFLAG_APPEND, IStorage::TYPE_SAVE);
	if(!File)
	{
		log_error("antibot", "failed to open file %s", pFilename);
		return;
	}

	const char *pAddr = "null";
	if(m_pRoundData)
		pAddr = m_pRoundData->m_aPlayers[ClientId].m_aAddress;

	char aLine[512];
	str_format(aLine, sizeof(aLine), "%d, %s, %s, %s", EventId, pAddr, ClientName(ClientId), pInfo ? pInfo : "");
	io_write(File, aLine, str_length(aLine));
	io_write_newline(File);
	io_close(File);
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

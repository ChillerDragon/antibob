#include <polybob/antibot/antibot_data.h>
#include <polybob/base/log.h>
#include <polybob/base/logger.h>
#include <polybob/base/system.h>
#include <polybob/engine/message.h>
#include <polybob/engine/shared/http.h>
#include <polybob/engine/shared/packer.h>
#include <polybob/engine/shared/protocol.h>
#include <polybob/engine/shared/protocol_ex.h>
#include <polybob/engine/storage.h>
#include <polybob/game/generated/protocol.h>
#include <polybob/game/generated/protocol7.h>
#include <polybob/game/generated/protocolglue.h>

#include <bob/antibot_player.h>
#include <bob/console.h>
#include <bob/detection_event.h>
#include <bob/gameserver.h>
#include <bob/version.h>

#include <cstdio>
#include <cstring>

#include "antibob.h"

CAntibob::CAntibob(CAntibotData *pData) :
	CGameServer(pData)
{
	mem_zero(m_apPlayers, sizeof(m_apPlayers));
}

//
// internal helpers
//

void CAntibob::RegisterCommands()
{
#define CONSOLE_COMMAND(name, params, callback, user, help) Console()->Register(name, params, callback, user, help);
#include <bob/commands.h>
#undef CONSOLE_COMMAND

	std::vector<CBobParam> vParams;
	char aBuf[1024];
#define CONSOLE_COMMAND(name, params, callback, user, help) \
	if(!CBobConsole::ParseParams(vParams, params, aBuf, sizeof(aBuf))) \
	{ \
		LogError("invalid antibot param check commands.h: %s", aBuf); \
		dbg_break(); \
	}
#include <bob/commands.h>
#undef CONSOLE_COMMAND
}

//
// rcon commands
//

void CAntibob::RconDump(const char *pSearch)
{
	if(!m_pRoundData)
	{
		LogError("missing round data");
		return;
	}

	for(int i = 0; i < ANTIBOT_MAX_CLIENTS; i++)
	{
		CAntibotPlayer *pPlayer = m_apPlayers[i];
		if(!pPlayer)
			continue;

		const char *pName = m_pRoundData->m_aCharacters[i].m_aName;
		if(pSearch[0] && !str_find_nocase(pName, pSearch))
			continue;

		char aEvents[512];
		aEvents[0] = '\0';
		if(pPlayer->m_DetectionEvents.size() != 0)
			CDetectionEvent::EventsToIdStr(pPlayer->m_DetectionEvents, aEvents, sizeof(aEvents));

		LogInfo("cid=%d name='%s' %s", i, pName, aEvents);
	}
}

void CAntibob::RconEvents(int ClientId)
{
	if(!m_pRoundData)
	{
		LogError("missing round data");
		return;
	}

	if(ClientId < 0 || ClientId >= ANTIBOT_MAX_CLIENTS)
	{
		LogError("client id out of range");
		return;
	}

	CAntibotPlayer *pPlayer = m_apPlayers[ClientId];
	if(!pPlayer)
	{
		LogInfo("client id %d is not connected", ClientId);
		return;
	}

	if(pPlayer->m_DetectionEvents.size() == 0)
	{
		LogInfo("player '%s' did not trigger any detections yet", ClientName(ClientId));
		return;
	}

	LogInfo("detection events for '%s'", ClientName(ClientId));
	for(const auto &[EventId, Event] : pPlayer->m_DetectionEvents)
	{
		LogInfo(
			"  event_id=%d name='%s'",
			EventId,
			Event.ToString());
		LogInfo(
			"   first_seen=%ds last_seen=%ds num_seen=%d",
			Event.SecondsSinceFirstTrigger(),
			Event.SecondsSinceLastTrigger(),
			Event.m_Amount);
		if(Event.m_aInfo[0])
			LogInfo("   %s", Event.m_aInfo);
	}
}

//
// antibob special hooks
//

bool CAntibob::OnSayNetMessage(const polybob::CNetMsg_Cl_Say *pMsg, int ClientId, const polybob::CUnpacker *pUnpacker)
{
	if(str_find_nocase(pMsg->m_pMessage, "i am using a cheat client"))
		if(Config()->m_AbAutoKick)
			Kick(ClientId, Config()->m_AbKickReason[0] ? Config()->m_AbKickReason : "self report");
	if(str_find_nocase(pMsg->m_pMessage, "i hack"))
		Detect(ClientId, BOB_DE_SELFREPORT, "said 'i hack'");
	// if(str_find_nocase(pMsg->m_pMessage, "uwu"))
	// 	Punish(ClientId, "no uwu allowed", 0, CPendingPunish::EPunish::KICK);
	return false;
}

bool CAntibob::OnSayNetMessage7(const polybob::protocol7::CNetMsg_Cl_Say *pMsg, int ClientId, const polybob::CUnpacker *pUnpacker)
{
	if(pMsg->m_Mode == polybob::protocol7::CHAT_WHISPER)
		if(str_find_nocase(pMsg->m_pMessage, "i use a cheat"))
			Kick(ClientId, "self report");
	return false;
}

void CAntibob::OnPlayerConnect(CAntibotPlayer *pPlayer)
{
	log_info("ab", "connect");
	std::shared_ptr<CHttpRequest> pHttp = HttpGet("http://127.0.0.1:9090");
	pHttp->LogProgress(HTTPLOG::FAILURE);
	pHttp->IpResolve(IPRESOLVE::V4);
	pHttp->Timeout(CTimeout{4000, 15000, 500, 5});
	pHttp->HeaderString("Content-Type", "application/json");
	m_Http.Run(pHttp);
	log_info("ab", "http requested");

	// log_info("antibot", "'%s' joined the game", ClientName(pPlayer->m_ClientId));

	// const char *pFilename = "antibob_data.txt";
	// IOHANDLE File = Storage()->OpenFile(pFilename, IOFLAG_APPEND, IStorage::TYPE_SAVE);
	// if(!File)
	// {
	// 	log_error("antibot", "failed to open file %s", pFilename);
	// 	return;
	// }

	// char aLine[512];
	// str_format(aLine, sizeof(aLine), "player join: %s", ClientName(pPlayer->m_ClientId));
	// io_write(File, aLine, str_length(aLine));
	// io_write_newline(File);
	// io_close(File);
}

//
// ddnet antibot interface hooks
//

void CAntibob::OnInit(CAntibotData *pData)
{
	log_info("antibob", "antibob antibot initialized");
	log_info("antibot", "antibob v" ANTIBOB_VERSION " git rev: %s, built on: " ANTIBOB_BUILD_DATE, BOB_GIT_SHORTREV_HASH);

	if(secure_random_init() != 0)
	{
		log_error("antibob", "could not initialize secure RNG");
		dbg_break();
	}

	RegisterCommands();
	m_ConfigManager.OnInit();
	m_Console.OnInit(&m_ConfigManager, this);
	m_PunishController.OnInit(this);
}

void CAntibob::OnRoundStart(CAntibotRoundData *pRoundData)
{
	m_pRoundData = pRoundData;
}

void CAntibob::OnRoundEnd()
{
	m_pRoundData = nullptr;
}

void CAntibob::OnUpdateData()
{
}

void CAntibob::OnDestroy()
{
}

bool CAntibob::OnConsoleCommand(const char *pCommand)
{
	if(!Console()->ExecuteCmd(pCommand))
	{
		LogInfo("unknown antibot command '%s' see 'antibot cmdlist' for a full list", pCommand);
		return false;
	}
	return true;
}

void CAntibob::OnPlayerInit(int ClientId)
{
}

void CAntibob::OnPlayerDestroy(int ClientId)
{
}

void CAntibob::OnSpawn(int ClientId)
{
	// if(!m_BobAbi.Rcon("say player spawned"))
	// 	log_error("antibot", "server does not support antibob rcon extension");
}

void CAntibob::OnHammerFireReloading(int ClientId)
{
}

void CAntibob::OnHammerFire(int ClientId)
{
}

void CAntibob::OnHammerHit(int ClientId, int TargetId)
{
}

void CAntibob::OnDirectInput(int ClientId)
{
}

void CAntibob::OnCharacterTick(int ClientId)
{
}

void CAntibob::OnHookAttach(int ClientId, bool Player)
{
}

void CAntibob::OnEngineTick()
{
	m_PunishController.OnTick();
}

void CAntibob::OnEngineClientJoin(int ClientId)
{
	bool Sixup = m_pRoundData->m_aPlayers[ClientId].m_Sixup;
	m_apPlayers[ClientId] = new CAntibotPlayer(ClientId, m_NextUniqueClientId, Sixup);
	m_NextUniqueClientId += 1;
	m_Network.OnClientConnect(ClientId, Sixup);
}

void CAntibob::OnEngineClientDrop(int ClientId, const char *pReason)
{
	m_PunishController.OnPlayerDisconnect(ClientId);

	delete m_apPlayers[ClientId];
	m_apPlayers[ClientId] = nullptr;

	m_Network.OnClientDisconnect(ClientId);
}

bool CAntibob::OnEngineClientMessage(int ClientId, const void *pData, int Size, int Flags)
{
	return m_Network.OnEngineClientMessage(ClientId, pData, Size, Flags, this);
}

bool CAntibob::OnEngineServerMessage(int ClientId, const void *pData, int Size, int Flags)
{
	return false;
}

bool CAntibob::OnEngineSimulateClientMessage(int *pClientId, void *pBuffer, int BufferSize, int *pOutSize, int *pFlags)
{
	return false;
}

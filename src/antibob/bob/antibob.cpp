#include <polybob/antibot/antibot_data.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/engine/message.h>
#include <polybob/engine/shared/packer.h>
#include <polybob/engine/shared/protocol.h>
#include <polybob/engine/shared/protocol_ex.h>
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
		exit(1); \
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
			Kick(ClientId, "self report");
	if(str_find_nocase(pMsg->m_pMessage, "i hack"))
		m_apPlayers[ClientId]->Detect(BOB_DE_SELFREPORT, "said 'i hack'");
	return false;
}

bool CAntibob::OnSayNetMessage7(const polybob::protocol7::CNetMsg_Cl_Say *pMsg, int ClientId, const polybob::CUnpacker *pUnpacker)
{
	if(pMsg->m_Mode == polybob::protocol7::CHAT_WHISPER)
		if(str_find_nocase(pMsg->m_pMessage, "i use a cheat"))
			Kick(ClientId, "self report");
	return false;
}

//
// ddnet antibot interface hooks
//

void CAntibob::OnInit(CAntibotData *pData)
{
	LogInfo("antibob antibot initialized");
	LogInfo("git revision hash: %s", BOB_GIT_SHORTREV_HASH);
	RegisterCommands();
	m_ConfigManager.OnInit();
	m_Console.OnInit(&m_ConfigManager, this);
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
}

void CAntibob::OnHammerFireReloading(int ClientId)
{
}

void CAntibob::OnHammerFire(int ClientId)
{
	// SendChatTarget(ClientId, "you hammered");
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
	// char aBuf[512];
	// str_format(aBuf, sizeof(aBuf), "'%s' hooked", ClientName(ClientId));
	// SendChat(-1, TEAM_ALL, aBuf);
}

void CAntibob::OnEngineTick()
{
}

void CAntibob::OnEngineClientJoin(int ClientId)
{
	bool Sixup = m_pRoundData->m_aPlayers[ClientId].m_Sixup;
	m_apPlayers[ClientId] = new CAntibotPlayer(ClientId, Sixup);
	m_Network.OnClientConnect(ClientId, Sixup);
}

void CAntibob::OnEngineClientDrop(int ClientId, const char *pReason)
{
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

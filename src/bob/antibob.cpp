#include <antibot/antibot_data.h>
#include <base/log.h>
#include <base/system.h>
#include <bob/detection_event.h>
#include <engine/message.h>
#include <engine/shared/packer.h>
#include <engine/shared/protocol.h>
#include <engine/shared/protocol_ex.h>
#include <engine/shared/uuid_manager.h>
#include <game/generated/protocol.h>
#include <game/generated/protocol7.h>
#include <game/generated/protocolglue.h>

#include <bob/antibot_player.h>
#include <bob/console.h>
#include <bob/gameserver.h>

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
#define CONSOLE_COMMAND(name, params, callback, user, help) dbg_assert(CBobConsole::ParseParams(vParams, params), "invalid antibot param check commands.h");
#include <bob/commands.h>
#undef CONSOLE_COMMAND
}

void CAntibob::RegisterUuids()
{
#define UUID(id, name) g_UuidManager.RegisterName(id, name);
#include <bob/protocol_ex_msgs.h>
#undef UUID
}

//
// rcon commands
//

void CAntibob::RconDump(const char *pSearch)
{
	if(!m_pRoundData)
	{
		log_error("antibot", "missing round data");
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

		log_info("antibot", "cid=%d name='%s' %s", i, pName, aEvents);
	}
}

void CAntibob::RconEvents(int ClientId)
{
	if(!m_pRoundData)
	{
		log_error("antibot", "missing round data");
		return;
	}

	if(ClientId < 0 || ClientId >= ANTIBOT_MAX_CLIENTS)
	{
		log_error("antibot", "client id out of range");
		return;
	}

	CAntibotPlayer *pPlayer = m_apPlayers[ClientId];
	if(!pPlayer)
	{
		log_info("antibot", "client id %d is not connected", ClientId);
		return;
	}

	if(pPlayer->m_DetectionEvents.size() == 0)
	{
		log_info("antibot", "player '%s' did not trigger any detections yet", ClientName(ClientId));
		return;
	}

	log_info("antibot", "detection events for '%s'", ClientName(ClientId));
	for(const auto &[EventId, Event] : pPlayer->m_DetectionEvents)
	{
		log_info(
			"antibot",
			"  event_id=%d name='%s'",
			EventId,
			Event.ToString());
		log_info(
			"antibot",
			"   first_seen=%ds last_seen=%ds num_seen=%d",
			Event.SecondsSinceFirstTrigger(),
			Event.SecondsSinceLastTrigger(),
			Event.m_Amount);
		if(Event.m_aInfo[0])
			log_info("antibot", "   %s", Event.m_aInfo);
	}
}

//
// antibob special hooks
//

bool CAntibob::OnSayNetMessage(const CNetMsg_Cl_Say *pMsg, int ClientId, const CUnpacker *pUnpacker)
{
	if(str_find_nocase(pMsg->m_pMessage, "i am using a cheat client"))
		Kick(ClientId, "self report");
	if(str_find_nocase(pMsg->m_pMessage, "i hack"))
		m_apPlayers[ClientId]->Detect(BOB_DE_SELFREPORT, "said 'i hack'");
	return false;
}

bool CAntibob::OnSayNetMessage7(const protocol7::CNetMsg_Cl_Say *pMsg, int ClientId, const CUnpacker *pUnpacker)
{
	if(pMsg->m_Mode == protocol7::CHAT_WHISPER)
		if(str_find_nocase(pMsg->m_pMessage, "i use a cheat"))
			Kick(ClientId, "self report");
	return false;
}

//
// ddnet antibot interface hooks
//

void CAntibob::OnInit(CAntibotData *pData)
{
	log_info("antibot", "antibob antibot initialized");
	RegisterCommands();
	RegisterUuids();
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
		log_info("antibot", "unknown antibot command '%s' see 'antibot cmdlist' for a full list", pCommand);
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

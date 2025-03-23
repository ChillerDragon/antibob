#include <antibot/antibot_data.h>
#include <base/log.h>
#include <base/system.h>
#include <engine/shared/protocol.h>
#include <game/generated/protocol.h>
#include <game/generated/protocol7.h>
#include <game/generated/protocolglue.h>

#include <cstdio>
#include <cstring>

#include "antibot_player.h"
#include "base.h"

#include "antibob.h"

CAntibob::CAntibob(CAntibotData *pData) :
	CBase(pData)
{
	mem_zero(m_apPlayers, sizeof(m_apPlayers));
}

void CAntibob::OnInit(CAntibotData *pData)
{
	log_info("antibot", "antibob antibot initialized");
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
	if(strcmp(pCommand, "dump") == 0)
	{
		log_info("antibot", "null antibot");
		if(m_pRoundData)
		{
			for(int i = 0; i < ANTIBOT_MAX_CLIENTS; i++)
			{
				if(!m_apPlayers[i])
					continue;

				log_info("antibot", "%s", m_pRoundData->m_aCharacters[i].m_aName);
			}
		}
	}
	else
	{
		// log_info("antibot", "unknown command");
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
	SendChatTarget(ClientId, "you spawned");
}

void CAntibob::OnHammerFireReloading(int ClientId)
{
}

void CAntibob::OnHammerFire(int ClientId)
{
	SendChatTarget(ClientId, "you hammered");
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
	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), "'%s' hooked", ClientName(ClientId));
	SendChat(-1, TEAM_ALL, aBuf);
}

void CAntibob::OnEngineTick()
{
}

void CAntibob::OnEngineClientJoin(int ClientId, bool Sixup)
{
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
	return false;
}

bool CAntibob::OnEngineServerMessage(int ClientId, const void *pData, int Size, int Flags)
{
	return false;
}

bool CAntibob::OnEngineSimulateClientMessage(int *pClientId, void *pBuffer, int BufferSize, int *pOutSize, int *pFlags)
{
	return false;
}

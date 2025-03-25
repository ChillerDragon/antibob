#include <antibot/antibot_data.h>
#include <base/log.h>
#include <base/system.h>
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
// rcon commands
//

void CAntibob::DumpPlayers(const char *pSearch)
{
	if(!m_pRoundData)
	{
		log_error("antibot", "missing round data");
		return;
	}

	for(int i = 0; i < ANTIBOT_MAX_CLIENTS; i++)
	{
		if(!m_apPlayers[i])
			continue;

		const char *pName = m_pRoundData->m_aCharacters[i].m_aName;
		if(pSearch[0] && !str_find_nocase(pName, pSearch))
			continue;

		log_info("antibot", "cid=%d name='%s'", i, pName);
	}
}

//
// antibob special hooks
//

void CAntibob::OnSayNetMessage(const CNetMsg_Cl_Say *pMsg, int ClientId, const CUnpacker *pUnpacker)
{
	if(str_find_nocase(pMsg->m_pMessage, "i am using a cheat client"))
		Kick(ClientId, "self report");
}

//
// ddnet antibot interface hooks
//

void CAntibob::OnInit(CAntibotData *pData)
{
	log_info("antibot", "antibob antibot initialized");

#define CONSOLE_COMMAND(name, params, callback, user, help) Console()->Register(name, params, callback, user, help);
#include <bob/commands.h>
#undef CONSOLE_COMMAND

	std::vector<CBobParam> vParams;
#define CONSOLE_COMMAND(name, params, callback, user, help) dbg_assert(CBobConsole::ParseParams(vParams, params), "invalid antibot param check commands.h");
#include <bob/commands.h>
#undef CONSOLE_COMMAND
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
	// TODO: ddnet does a Server()->ClientIngame(ClientId)
	//       check here. We probably need that too otherwise clients can
	//       send messages to early in their connection phase
	if(!m_apPlayers[ClientId])
		return false;

	// TODO: use PreProcessMsg()
	// TODO: think about 0.7 whisper
	if(m_Network.IsSixup(ClientId))
		return false;

	CMsgPacker Packer(NETMSG_EX);
	Packer.Reset();

	CUnpacker Unpacker;
	Unpacker.Reset(pData, Size);

	int Msg;
	bool Sys;
	CUuid Uuid;

	int Result = UnpackMessageId(&Msg, &Sys, &Uuid, &Unpacker, &Packer);
	if(Result == UNPACKMESSAGE_ERROR)
		return false;

	void *pRawMsg = m_Network.m_NetObjHandler.SecureUnpackMsg(Msg, &Unpacker);
	if(!pRawMsg)
		return false;

	if(Msg == NETMSGTYPE_CL_SAY)
		OnSayNetMessage(static_cast<CNetMsg_Cl_Say *>(pRawMsg), ClientId, &Unpacker);
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

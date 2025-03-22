#include <base/log.h>
#include <engine/shared/protocol.h>
#include <game/generated/protocol.h>
#include <game/generated/protocol7.h>
#include <game/generated/protocolglue.h>

#include <cstdio>
#include <cstring>

#include "antibob.h"

void CAntibob::OnInit(CAntibotData *pData)
{
	m_pData = pData;
	log_info("antibot", "antibob antibot initialized");
}

void CAntibob::SendChatTarget(int ClientId, const char *pMessage)
{
	CNetMsg_Sv_Chat Msg;
	Msg.m_Team = 0;
	Msg.m_ClientId = -1;
	Msg.m_pMessage = pMessage;

	// TODO: move this into a send msg helper

	CMsgPacker MsgPacker(CNetMsg_Sv_Chat::ms_MsgId, false, false);
	Msg.Pack(&MsgPacker);

	bool System = false;
	CPacker Packer;
	Packer.Reset();
	Packer.AddInt((CNetMsg_Sv_Chat::ms_MsgId << 1) | (System ? 1 : 0));
	Packer.AddRaw(MsgPacker.Data(), MsgPacker.Size());

	m_pData->m_pfnSend(ClientId, Packer.Data(), Packer.Size(), 0, m_pData->m_pUser);
}

void CAntibob::OnRoundStart(CAntibotRoundData *pRoundData)
{
}

void CAntibob::OnRoundEnd()
{
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
}

void CAntibob::OnEngineTick()
{
}

void CAntibob::OnEngineClientJoin(int ClientId, bool Sixup)
{
}

void CAntibob::OnEngineClientDrop(int ClientId, const char *pReason)
{
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

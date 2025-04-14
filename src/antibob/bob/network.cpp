#include <polybob/antibot/antibot_data.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/engine/shared/protocol.h>
#include <polybob/game/generated/protocolglue.h>

#include "bob/antibob.h"
#include "network.h"

void CNetwork::OnInit(CAntibotData *pData)
{
	m_pData = pData;
}

bool CNetwork::RepackMsg(const CMsgPacker *pMsg, CPacker &Packer, bool Sixup)
{
	int MsgId = pMsg->m_MsgId;
	Packer.Reset();

	if(Sixup && !pMsg->m_NoTranslate)
	{
		if(pMsg->m_System)
		{
			if(MsgId >= OFFSET_UUID)
				;
			else if(MsgId >= NETMSG_MAP_CHANGE && MsgId <= NETMSG_MAP_DATA)
				;
			else if(MsgId >= NETMSG_CON_READY && MsgId <= NETMSG_INPUTTIMING)
				MsgId += 1;
			else if(MsgId == NETMSG_RCON_LINE)
				MsgId = protocol7::NETMSG_RCON_LINE;
			else if(MsgId >= NETMSG_PING && MsgId <= NETMSG_PING_REPLY)
				MsgId += 4;
			else if(MsgId >= NETMSG_RCON_CMD_ADD && MsgId <= NETMSG_RCON_CMD_REM)
				MsgId -= 11;
			else
			{
				log_error("net", "DROP send sys %d", MsgId);
				return false;
			}
		}
		else
		{
			if(MsgId >= 0 && MsgId < OFFSET_UUID)
				MsgId = Msg_SixToSeven(MsgId);

			if(MsgId < 0)
				return false;
		}
	}

	if(MsgId < OFFSET_UUID)
	{
		Packer.AddInt((MsgId << 1) | (pMsg->m_System ? 1 : 0));
	}
	else
	{
		Packer.AddInt(pMsg->m_System ? 1 : 0); // NETMSG_EX, NETMSGTYPE_EX
		g_BobUuidManager.PackUuid(MsgId, &Packer);
	}
	Packer.AddRaw(pMsg->Data(), pMsg->Size());

	return true;
}

bool CNetwork::SendMsg(CMsgPacker *pMsg, int Flags, int ClientId)
{
	CPacker Packer;
	if(!RepackMsg(pMsg, Packer, m_aClients[ClientId].m_Sixup))
		return false;

	m_pData->m_pfnSend(ClientId, Packer.Data(), Packer.Size(), 0, m_pData->m_pUser);
	return true;
}

void CNetwork::OnClientConnect(int ClientId, bool Sixup)
{
	m_aClients[ClientId].Init(ClientId, Sixup);
}

void CNetwork::OnClientDisconnect(int ClientId)
{
	m_aClients[ClientId].m_Active = false;
}

bool CNetwork::OnEngineClientMessage(int ClientId, const void *pData, int Size, int Flags, class CAntibob *pAntibob)
{
	// TODO: ddnet does a Server()->ClientIngame(ClientId)
	//       check here. We probably need that too otherwise clients can
	//       send messages to early in their connection phase
	if(!pAntibob->m_apPlayers[ClientId])
		return false;

	CMsgPacker Packer(NETMSG_EX);
	Packer.Reset();

	CUnpacker Unpacker;
	Unpacker.Reset(pData, Size);

	int Msg;
	bool Sys;
	CUuid Uuid;

	int Result = BobUnpackMessageId(&Msg, &Sys, &Uuid, &Unpacker, &Packer);
	if(Result == UNPACKMESSAGE_ERROR)
		return false;

	if(Sys)
	{
		// if(Msg == BOB_NETMSG_PINGEX)
		// 	log_info("antibot", "got pingex");
		// else if(Msg == BOB_NETMSG_PONGEX)
		// 	log_info("antibot", "got pongex");
	}

	void *pRawMsg = nullptr;
	if(IsSixup(ClientId))
		pRawMsg = m_NetObjHandler7.SecureUnpackMsg(Msg, &Unpacker);
	else
		pRawMsg = m_NetObjHandler.SecureUnpackMsg(Msg, &Unpacker);

	if(!pRawMsg)
		return false;

	if(IsSixup(ClientId))
	{
		if(Msg == protocol7::NETMSGTYPE_CL_SAY)
			pAntibob->OnSayNetMessage7(static_cast<protocol7::CNetMsg_Cl_Say *>(pRawMsg), ClientId, &Unpacker);
	}
	else
	{
		if(Msg == NETMSGTYPE_CL_SAY)
			pAntibob->OnSayNetMessage(static_cast<CNetMsg_Cl_Say *>(pRawMsg), ClientId, &Unpacker);
	}

	return false;
}

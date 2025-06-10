#include <polybob/antibot/antibot_data.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/engine/shared/protocol.h>
#include <polybob/game/generated/protocol.h>
#include <polybob/game/generated/protocolglue.h>

#include "bob/antibob.h"
#include "bob/gameserver.h"
#include "network.h"

using namespace polybob;

void CNetwork::OnInit(CAntibotData *pData, CGameServer *pGameServer)
{
	m_pData = pData;
	m_pGameServer = pGameServer;
}

bool CNetwork::RepackMsg(const CMsgPacker *pMsg, polybob::CPacker &Packer, bool Sixup)
{
	int MsgId = pMsg->m_MsgId;
	Packer.Reset();

	if(Sixup && !pMsg->m_NoTranslate)
	{
		if(pMsg->m_System)
		{
			if(MsgId >= OFFSET_UUID)
				;
			else if(MsgId >= polybob::NETMSG_MAP_CHANGE && MsgId <= polybob::NETMSG_MAP_DATA)
				;
			else if(MsgId >= polybob::NETMSG_CON_READY && MsgId <= polybob::NETMSG_INPUTTIMING)
				MsgId += 1;
			else if(MsgId == polybob::NETMSG_RCON_LINE)
				MsgId = polybob::protocol7::NETMSG_RCON_LINE;
			else if(MsgId >= polybob::NETMSG_PING && MsgId <= polybob::NETMSG_PING_REPLY)
				MsgId += 4;
			else if(MsgId >= polybob::NETMSG_RCON_CMD_ADD && MsgId <= polybob::NETMSG_RCON_CMD_REM)
				MsgId -= 11;
			else
			{
				m_pGameServer->LogError("DROP send sys %d", MsgId);
				return false;
			}
		}
		else
		{
			if(MsgId >= 0 && MsgId < OFFSET_UUID)
				MsgId = polybob::Msg_SixToSeven(MsgId);

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
	polybob::CPacker Packer;
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

	CMsgPacker Packer(polybob::NETMSG_EX);
	Packer.Reset();

	polybob::CUnpacker Unpacker;
	Unpacker.Reset(pData, Size);

	int Msg;
	bool Sys;
	CUuid Uuid;

	int Result = BobUnpackMessageId(&Msg, &Sys, &Uuid, &Unpacker, &Packer);
	if(Result == UNPACKMESSAGE_ERROR)
		return false;

	if(Sys)
	{
		if(IsSixup(ClientId))
		{
			if(Msg == polybob::protocol7::NETMSG_READY)
				Msg = polybob::NETMSG_READY;
			else if(Msg == polybob::protocol7::NETMSG_ENTERGAME)
				Msg = polybob::NETMSG_ENTERGAME;
			else if(Msg == polybob::protocol7::NETMSG_INPUT)
				Msg = polybob::NETMSG_INPUT;
		}

		if(Msg == polybob::NETMSG_READY)
		{
			if(m_aClients[ClientId].m_State == CAntibotClient::EState::CONNECTING)
			{
				m_aClients[ClientId].m_State = CAntibotClient::EState::READY;
			}
		}
		else if(Msg == polybob::NETMSG_ENTERGAME && pAntibob->IsClientReady(ClientId))
		{
			if(m_aClients[ClientId].m_State == CAntibotClient::EState::READY)
			{
				m_aClients[ClientId].m_State = CAntibotClient::EState::PREINGAME;
			}
		}
		else if(Msg == polybob::NETMSG_INPUT)
		{
			if(m_aClients[ClientId].m_State == CAntibotClient::EState::PREINGAME)
			{
				m_aClients[ClientId].m_State = CAntibotClient::EState::INGAME;
				pAntibob->OnPlayerConnect(pAntibob->m_apPlayers[ClientId]);
			}

			const int LastAckedSnapshot = Unpacker.GetInt();
			int IntendedTick = Unpacker.GetInt();
			int Size = Unpacker.GetInt();
			if(Unpacker.Error() || Size / 4 > polybob::MAX_INPUT_SIZE || IntendedTick < polybob::MIN_TICK || IntendedTick >= polybob::MAX_TICK)
				return false;

			int aData[MAX_INPUT_SIZE];
			for(int i = 0; i < Size / 4; i++)
				aData[i] = Unpacker.GetInt();
			if(Unpacker.Error())
				return false;

			pAntibob->OnInputNetMessage(ClientId, LastAckedSnapshot, IntendedTick, Size, (CNetObj_PlayerInput *)aData);
		}
	}
	else // game msg
	{
		void *pRawMsg = nullptr;
		if(IsSixup(ClientId))
			pRawMsg = m_NetObjHandler7.SecureUnpackMsg(Msg, &Unpacker);
		else
			pRawMsg = m_NetObjHandler.SecureUnpackMsg(Msg, &Unpacker);

		if(!pRawMsg)
		{
			// log_error(
			// 	"antibob",
			// 	"sixup=%d sys=%d msgid=%d secure unpack error: %s",
			// 	IsSixup(ClientId),
			// 	Sys,
			// 	Msg,
			// 	IsSixup(ClientId) ? m_NetObjHandler7.FailedMsgOn() : m_NetObjHandler.FailedMsgOn());
			return false;
		}

		if(IsSixup(ClientId))
		{
			if(Msg == polybob::protocol7::NETMSGTYPE_CL_SAY)
				return pAntibob->OnSayNetMessage7(static_cast<polybob::protocol7::CNetMsg_Cl_Say *>(pRawMsg), ClientId, &Unpacker);
		}
		else
		{
			if(Msg == polybob::NETMSGTYPE_CL_SAY)
				return pAntibob->OnSayNetMessage(static_cast<polybob::CNetMsg_Cl_Say *>(pRawMsg), ClientId, &Unpacker);
			else if(Msg == polybob::NETMSGTYPE_CL_STARTINFO)
				pAntibob->m_apPlayers[ClientId]->m_Ready = true;
		}
	}
	return false;
}

int CNetwork::Tick() const
{
	if(!m_pGameServer)
		return 0;
	if(!m_pGameServer->m_pRoundData)
		return 0;

	return m_pGameServer->m_pRoundData->m_Tick;
}

#include <game/generated/protocol.h>

#include "base.h"

void CBase::SendChatTarget(int ClientId, const char *pMessage)
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

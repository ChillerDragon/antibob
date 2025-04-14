#pragma once

#include <polybob/antibot/antibot_data.h>
#include <polybob/base/system.h>
#include <polybob/engine/message.h>
#include <polybob/engine/shared/packer.h>
#include <polybob/game/generated/protocol.h>
#include <polybob/game/generated/protocol7.h>

class CAntibotClient
{
public:
	// if active is false this client is not connected
	// and all values are garbage
	bool m_Active = false;

	int m_ClientId = -1;
	bool m_Sixup = false;
	// char m_aName[MAX_NAME_LENGTH];

	void Init(int ClientId, bool Sixup)
	{
		m_Active = true;

		m_ClientId = ClientId;
		m_Sixup = Sixup;
		// str_copy(m_aName, "(connecting)");
	}
};

class CNetwork
{
	CAntibotData *m_pData = nullptr;

public:
	CAntibotClient m_aClients[ANTIBOT_MAX_CLIENTS];
	CNetObjHandler m_NetObjHandler;
	protocol7::CNetObjHandler m_NetObjHandler7;

	void OnInit(CAntibotData *pData);
	void OnClientConnect(int ClientId, bool Sixup);
	void OnClientDisconnect(int ClientId);

	bool OnEngineClientMessage(int ClientId, const void *pData, int Size, int Flags, class CAntibob *pAntibob);

	bool IsSixup(int ClientId) { return m_aClients[ClientId].m_Sixup; }
	// const char *ClientName(int ClientId) { return m_aClients[ClientId].m_aName; }
	int MaxClients() { return ANTIBOT_MAX_CLIENTS; }
	bool ClientIngame(int ClientId) { return m_aClients[ClientId].m_Active; }

	static bool RepackMsg(const CMsgPacker *pMsg, CPacker &Packer, bool Sixup);
	bool SendMsg(CMsgPacker *pMsg, int Flags, int ClientId);

	template<class T, typename std::enable_if<!protocol7::is_sixup<T>::value, int>::type = 0>
	int SendPackMsg(const T *pMsg, int Flags, int ClientId)
	{
		int Result = 0;
		if(ClientId == -1)
		{
			for(int i = 0; i < MaxClients(); i++)
				if(ClientIngame(i))
					Result = SendPackMsgTranslate(pMsg, Flags, i);
		}
		else
		{
			Result = SendPackMsgTranslate(pMsg, Flags, ClientId);
		}
		return Result;
	}

	template<class T>
	int SendPackMsgOne(const T *pMsg, int Flags, int ClientId)
	{
		dbg_assert(ClientId != -1, "SendPackMsgOne called with -1");
		CMsgPacker Packer(T::ms_MsgId, false, protocol7::is_sixup<T>::value);

		if(pMsg->Pack(&Packer))
			return -1;
		return SendMsg(&Packer, Flags, ClientId);
	}

	int SendPackMsgTranslate(const CNetMsg_Sv_Chat *pMsg, int Flags, int ClientId)
	{
		CNetMsg_Sv_Chat MsgCopy;
		mem_copy(&MsgCopy, pMsg, sizeof(MsgCopy));

		// TODO: do not crash 0.6 vanilla clients
		//       this needs access to the id map and ddnet client version
		//       to properly work
		// char aBuf[1000];
		// if(MsgCopy.m_ClientId >= 0 && !Translate(MsgCopy.m_ClientId, ClientId))
		// {
		// 	str_format(aBuf, sizeof(aBuf), "%s: %s", ClientName(MsgCopy.m_ClientId), MsgCopy.m_pMessage);
		// 	MsgCopy.m_pMessage = aBuf;
		// 	MsgCopy.m_ClientId = VANILLA_MAX_CLIENTS - 1;
		// }

		if(IsSixup(ClientId))
		{
			protocol7::CNetMsg_Sv_Chat Msg7;
			Msg7.m_ClientId = MsgCopy.m_ClientId;
			Msg7.m_pMessage = MsgCopy.m_pMessage;
			Msg7.m_Mode = MsgCopy.m_Team > 0 ? protocol7::CHAT_TEAM : protocol7::CHAT_ALL;
			Msg7.m_TargetId = -1;
			return SendPackMsgOne(&Msg7, Flags, ClientId);
		}

		return SendPackMsgOne(&MsgCopy, Flags, ClientId);
	}
};

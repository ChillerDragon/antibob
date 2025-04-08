#pragma once

#include <antibot/antibot_data.h>
#include <engine/message.h>

#include <bob/antibot_player.h>
#include <bob/config.h>
#include <bob/console.h>
#include <bob/network.h>

class CGameServer
{
public:
	CGameServer(CAntibotData *pData);

	CAntibotData *m_pData = nullptr;
	CNetwork m_Network;
	CNetwork *Server() { return &m_Network; }
	CBobConsole m_Console;
	CBobConsole *Console() { return &m_Console; }
	CBobConfig *Config() const { return &g_BobConfig; }

	CAntibotRoundData *m_pRoundData = nullptr;
	CAntibotPlayer *m_apPlayers[ANTIBOT_MAX_CLIENTS];
	const char *ClientName(int ClientId) const { return !m_pRoundData ? "(null)" : m_pRoundData->m_aCharacters[ClientId].m_aName; }

	void SendChat(int ClientId, int Team, const char *pMessage);
	void SendChatTarget(int ClientId, const char *pMessage);

	//
	// antibot callbacks
	//

	void Kick(int ClientId, const char *pReason = nullptr) const;
};

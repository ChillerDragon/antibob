#pragma once

#include <polybob/antibot/antibot_data.h>
#include <polybob/base/system.h>
#include <polybob/engine/message.h>
#include <polybob/engine/storage.h>

#include <bob/antibot_player.h>
#include <bob/config.h>
#include <bob/console.h>
#include <bob/network.h>

class CGameServer
{
public:
	CGameServer(CAntibotData *pData);
	~CGameServer();

	CAntibotData *m_pData = nullptr;
	polybob::IStorage *m_pStorage = nullptr;
	CNetwork m_Network;
	CNetwork *Server() { return &m_Network; }
	CBobConsole m_Console;
	CBobConsole *Console() { return &m_Console; }
	CBobConfig *Config() const { return &g_BobConfig; }
	polybob::IStorage *Storage() const { return m_pStorage; }

	CBobConfigManager m_ConfigManager;

	CAntibotRoundData *m_pRoundData = nullptr;
	CAntibotPlayer *m_apPlayers[ANTIBOT_MAX_CLIENTS];
	const char *ClientName(int ClientId) const { return !m_pRoundData ? "(null)" : m_pRoundData->m_aCharacters[ClientId].m_aName; }

	void SendChat(int ClientId, int Team, const char *pMessage);
	void SendChatTarget(int ClientId, const char *pMessage);

	//
	// antibot callbacks
	//

	void Kick(int ClientId, const char *pReason = nullptr) const;
	void LogInfo(const char *pFormat, ...)
		GNUC_ATTRIBUTE((format(printf, 2, 3)));
	void LogError(const char *pFormat, ...)
		GNUC_ATTRIBUTE((format(printf, 2, 3)));
};

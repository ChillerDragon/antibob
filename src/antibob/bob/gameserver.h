#pragma once

#include <polybob/antibot/antibot_data.h>
#include <polybob/base/system.h>
#include <polybob/engine/message.h>
#include <polybob/engine/shared/jobs.h>
#include <polybob/engine/storage.h>

#include <bob/antibob_abi.h>
#include <bob/antibot_player.h>
#include <bob/config.h>
#include <bob/console.h>
#include <bob/network.h>
#include <bob/pending_punish.h>

#include <cstdint>
#include <memory>

class CGameServer
{
public:
	CGameServer(CAntibotData *pData);
	~CGameServer();

	CAntibotData *m_pData = nullptr;
	polybob::IStorage *m_pStorage = nullptr;
	polybob::CJobPool m_JobPool;
	CNetwork m_Network;
	CNetwork *Server() { return &m_Network; }
	CBobConsole m_Console;
	CBobConsole *Console() { return &m_Console; }
	CBobConfig *Config() const { return &g_BobConfig; }
	polybob::IStorage *Storage() const { return m_pStorage; }
	CAntibobAbi m_BobAbi;
	CPunishController m_PunishController;

	CBobConfigManager m_ConfigManager;

	CAntibotRoundData *m_pRoundData = nullptr;
	CAntibotPlayer *m_apPlayers[ANTIBOT_MAX_CLIENTS];

	// starting 1 to make 0 the special value "no client id"
	uint32_t m_NextUniqueClientId = 1;

	// returns CAntibotPlayer or nullptr
	// based on a matching unique client id
	// the unique client ids start at 1 and are never reused during the
	// runtime of the server
	CAntibotPlayer *GetPlayerByUniqueClientId(int UniqueClientId);

	const char *ClientName(int ClientId) const { return !m_pRoundData ? "(null)" : m_pRoundData->m_aCharacters[ClientId].m_aName; }

	void AddJob(std::shared_ptr<polybob::IJob> pJob);

	void SendChat(int ClientId, int Team, const char *pMessage);
	void SendChatTarget(int ClientId, const char *pMessage);

	// schedules a punishment like kicking
	// it will not be executed directly
	// but with a delay of up to a few minutes to obfuscate the detection
	// they will also be applied instantly on disconnect to avoid bypass
	void Punish(int ClientId, const char *pReason, int TimeInMinutes, CPendingPunish::EPunish Punish);

	//
	// antibot callbacks
	//

	// it is recommended to use Punish() instead of Kick()
	void Kick(int ClientId, const char *pReason = nullptr) const;
	void LogInfo(const char *pFormat, ...)
		GNUC_ATTRIBUTE((format(printf, 2, 3)));
	void LogError(const char *pFormat, ...)
		GNUC_ATTRIBUTE((format(printf, 2, 3)));
};

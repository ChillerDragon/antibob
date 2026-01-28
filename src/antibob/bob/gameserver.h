#pragma once

#include <bob/antibob_abi.h>
#include <bob/antibot_player.h>
#include <bob/collision.h>
#include <bob/config.h>
#include <bob/console.h>
#include <bob/gameworld.h>
#include <bob/network.h>
#include <bob/pending_punish.h>
#include <polybob/antibot/antibot_data.h>
#include <polybob/base/system.h>
#include <polybob/engine/shared/http.h>
#include <polybob/engine/shared/jobs.h>
#include <polybob/engine/storage.h>

#include <cstdint>
#include <memory>

class CGameServer
{
public:
	CGameServer(CAntibotData *pData);
	virtual ~CGameServer();

	CAntibotData *m_pData = nullptr;
	polybob::IStorage *m_pStorage = nullptr;
	polybob::CJobPool m_JobPool;
	CNetwork m_Network;
	CNetwork *Server() { return &m_Network; }
	CBobGameWorld m_GameWorld;
	CBobGameWorld *GameWorld() { return &m_GameWorld; }
	CBobCollision m_Collision;
	CBobCollision *Collision() { return &m_Collision; }
	CBobConsole m_Console;
	CBobConsole *Console() { return &m_Console; }
	CBobConfig *Config() const { return &g_BobConfig; }
	polybob::IStorage *Storage() const { return m_pStorage; }
	CHttp m_Http;
	CHttp *Http() { return &m_Http; }
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
	bool IsClientReady(int ClientId) const;

	void AddJob(std::shared_ptr<polybob::IJob> pJob);

	void SendChat(int ClientId, int Team, const char *pMessage);
	void SendChatTarget(int ClientId, const char *pMessage);
	void SendBroadcast(const char *pText, int ClientId);

	// schedules a punishment like kicking
	// it will not be executed directly
	// but with a delay of up to a few minutes to obfuscate the detection
	// they will also be applied instantly on disconnect to avoid bypass
	void Punish(int ClientId, const char *pReason, int TimeInMinutes, CPendingPunish::EPunish Punish);

	// register suspicious activity detection
	//
	// ClientId - players client id
	// EventId - event id should be defined in detection_events.h like BOB_DE_SAMPLE
	// pInfo - optional additional information about this specific occurrence (free text)
	void Detect(int ClientId, int EventId, const char *pInfo = nullptr, int Confidence = 90);

private:
	void LogEvent(int ClientId, int EventId, const char *pInfo = nullptr);

public:
	//
	// antibot callbacks
	//

	// it is recommended to use Punish() instead of Kick()
	void Kick(int ClientId, const char *pReason = nullptr) const;

	// This is bool because it might not ban
	bool Ban(int ClientId, int TimeInMinutes, const char *pReason = nullptr);
	bool Ban(const NETADDR &Ip, int TimeInMinutes, const char *pReason = nullptr);

	void LogInfo(const char *pFormat, ...)
		GNUC_ATTRIBUTE((format(printf, 2, 3)));
	void LogError(const char *pFormat, ...)
		GNUC_ATTRIBUTE((format(printf, 2, 3)));
};

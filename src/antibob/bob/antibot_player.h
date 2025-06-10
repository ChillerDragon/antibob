#pragma once

#include <bob/detection_event.h>
#include <polybob/game/generated/protocol.h>

#include <cstdint>
#include <unordered_map>

class CAntibotPlayer
{
public:
	CAntibotPlayer(int ClientId, uint32_t UniqueClientId, bool Sixup);

	// starting 1 to make 0 the special value "no client id"
	uint32_t m_UniqueClientId = 0;
	int m_ClientId;
	bool m_Sixup;
	polybob::CNetObj_PlayerInput m_aInputs[10];

	// same as ddnet's CPlayer::m_Ready
	// indicating if the client info was already sent
	bool m_Ready = false;

	std::unordered_map<int, CDetectionEvent> m_DetectionEvents;

	// registers a detection event
	// that will show up in the "dump_antibot" rcon command
	void Detect(int EventId, const char *pInfo = nullptr);

	//
	// Add new member variables for forks below this comment to avoid merge conflicts.
	//
};

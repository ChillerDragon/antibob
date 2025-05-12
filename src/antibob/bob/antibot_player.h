#pragma once

#include <bob/detection_event.h>

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
	std::unordered_map<int, CDetectionEvent> m_DetectionEvents;

	// registers a detection event
	// that will show up in the "dump_antibot" rcon command
	void Detect(int EventId, const char *pInfo = nullptr);

	//
	// Add new member variables for forks below this comment to avoid merge conflicts.
	//
};

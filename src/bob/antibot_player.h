#pragma once

#include <bob/detection_event.h>

#include <unordered_map>

class CAntibotPlayer
{
public:
	CAntibotPlayer(int ClientId, bool Sixup);

	int m_ClientId;
	bool m_Sixup;
	std::unordered_map<int, CDetectionEvent> m_DetectionEvents;

	// registers a detection event
	// that will show up in the "dump_antibot" rcon command
	void Detect(int EventId);

	//
	// Add new member variables for forks below this comment to avoid merge conflicts.
	//
};

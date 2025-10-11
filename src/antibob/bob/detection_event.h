#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

enum
{
#define MACRO_DETECTION_EVENT(EnumName, StrName, Desc) EnumName,
#include <bob/detection_events.h>
#undef MACRO_DETECTION_EVENT
	NUM_BOB_DETECTION_EVENTS,
};

// represents the event of a player
// being detected as cheater by the antibot system
class CDetectionEvent
{
	void Reset();

public:
	CDetectionEvent(int EventId);
	CDetectionEvent(int EventId, const char *pInfo, int Confidence);

	// events can be stacked to avoid spamming
	// the event log
	// if events get stacked this m_Amount is incremented
	int m_Amount = 1;

	// time_get() when the event was created
	int64_t m_FirstTriggerTime;
	int64_t m_LastTriggerTime;

	// type of event that got triggered
	// this number will show up in the rcon command
	// dump_antibot
	int m_EventId = BOB_DE_SAMPLE;

	// additional information about the event
	// might be empty
	char m_aInfo[1024];

	// value from 0-100 with the confidence
	// of it being a correct detection
	// 100 means this should never be a false positive
	// and 0 means this is most likely a false positive
	int m_Confidence;

	int SecondsSinceFirstTrigger() const;
	int SecondsSinceLastTrigger() const;

	// event id represented as human readable string
	const char *ToString() const;

	// turns event id into short friendly name
	static const char *EventToStr(int EventId);

	// truns event id into event description
	static const char *EventToDesc(int EventId);

	// given a list of events it writes a string into
	// pBuf that looks something like this "(2, 10, 4)"
	// where the numbers are the m_EventId values
	static void EventsToIdStr(const std::unordered_map<int, CDetectionEvent> &Events, int MinConfidence, char *pBuf, int BufSize);

	// pushes NewEvent into Events
	// but if the same event type is already known it just
	// updates that event to increment the amount of times we have seen it
	static void PushAndStack(std::unordered_map<int, CDetectionEvent> &Events, CDetectionEvent NewEvent);

	bool operator==(const CDetectionEvent &Other) const
	{
		return m_EventId == Other.m_EventId;
	}
};

#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

enum
{
	BOB_DE_SAMPLE,
	BOB_DE_SELFREPORT,
	NUM_BOB_DES,
};

// represents the event of a player
// being detected as cheater by the antibot system
class CDetectionEvent
{
	void Reset();

public:
	CDetectionEvent(int EventId);
	CDetectionEvent(int EventId, const char *pInfo);

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

	int SecondsSinceFirstTrigger() const;
	int SecondsSinceLastTrigger() const;

	// event id represented as human readable string
	const char *ToString() const;

	static const char *EventToStr(int EventId);

	// given a list of events it writes a string into
	// pBuf that looks something like this "(2, 10, 4)"
	// where the numbers are the m_EventId values
	static void EventsToIdStr(const std::unordered_map<int, CDetectionEvent> &Events, char *pBuf, int BufSize);

	// pushes NewEvent into Events
	// but if the same event type is already known it just
	// updates that event to increment the amount of times we have seen it
	static void PushAndStack(std::unordered_map<int, CDetectionEvent> &Events, CDetectionEvent NewEvent);

	bool operator==(const CDetectionEvent &Other) const
	{
		return m_EventId == Other.m_EventId;
	}
};

#include <polybob/base/system.h>
#include <unordered_map>

#include "detection_event.h"

CDetectionEvent::CDetectionEvent(int EventId) :
	m_EventId(EventId)
{
	Reset();
}

CDetectionEvent::CDetectionEvent(int EventId, const char *pInfo) :
	m_EventId(EventId)
{
	Reset();

	if(pInfo)
		str_copy(m_aInfo, pInfo);
}

void CDetectionEvent::Reset()
{
	m_aInfo[0] = '\0';
	m_FirstTriggerTime = time_get();
	m_LastTriggerTime = time_get();
}

int CDetectionEvent::SecondsSinceFirstTrigger() const
{
	return (time_get() - m_FirstTriggerTime) / time_freq();
}

int CDetectionEvent::SecondsSinceLastTrigger() const
{
	return (time_get() - m_LastTriggerTime) / time_freq();
}

const char *CDetectionEvent::ToString() const
{
	return EventToStr(m_EventId);
}

const char *CDetectionEvent::EventToStr(int EventId)
{
	switch(EventId)
	{
#define MACRO_DETECTION_EVENT(EnumName, StrName, Desc) \
	case EnumName: \
		return StrName;
#include <bob/detection_events.h>
#undef MACRO_DETECTION_EVENT
	}
	return "unknown";
}

const char *CDetectionEvent::EventToDesc(int EventId)
{
	switch(EventId)
	{
#define MACRO_DETECTION_EVENT(EnumName, StrName, Desc) \
	case EnumName: \
		return Desc;
#include <bob/detection_events.h>
#undef MACRO_DETECTION_EVENT
	}
	return "unknown";
}

void CDetectionEvent::EventsToIdStr(const std::unordered_map<int, CDetectionEvent> &Events, char *pBuf, int BufSize)
{
	int BufIdx = 0;
	auto SafeAppend = [&pBuf, BufSize, &BufIdx](const char *pText) -> bool {
		const int TextLen = str_length(pText);
		if(TextLen + BufIdx >= BufSize)
			return false;

		for(int i = 0; i < TextLen; i++)
			pBuf[BufIdx++] = pText[i];
		return true;
	};

	SafeAppend("(");
	int EventNum = 0;
	for(const auto &[EventId, _] : Events)
	{
		char aEvent[16];
		str_format_int(aEvent, sizeof(aEvent), EventId);
		if(!SafeAppend(aEvent))
			break;

		if(++EventNum != Events.size())
		{
			if(!SafeAppend(", "))
				break;
		}
	}
	SafeAppend(")");
	pBuf[BufIdx] = '\0';
}

void CDetectionEvent::PushAndStack(std::unordered_map<int, CDetectionEvent> &Events, CDetectionEvent NewEvent)
{
	auto Event = Events.find(NewEvent.m_EventId);

	// first of type
	if(Event == Events.end())
	{
		Events.emplace(NewEvent.m_EventId, NewEvent);
		return;
	}

	// known event
	Event->second.m_LastTriggerTime = time_get();
	Event->second.m_Amount++;
}

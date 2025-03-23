#pragma once

#include "network.h"
#include <antibot/antibot_data.h>
#include <engine/message.h>

class CBase
{
public:
	CAntibotData *m_pData = nullptr;
	CNetwork m_Network;
	CNetwork *Server() { return &m_Network; }

	CBase(CAntibotData *pData);

	void SendChatTarget(int ClientId, const char *pMessage);
};

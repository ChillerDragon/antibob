#pragma once

class CAntibotPlayer
{
public:
	CAntibotPlayer(int ClientId, bool Sixup) :
		m_ClientId(ClientId), m_Sixup(Sixup){};

	int m_ClientId;
	bool m_Sixup;

	//
	// Add new member variables for forks below this comment to avoid merge conflicts.
	//
};

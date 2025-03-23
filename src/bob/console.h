#pragma once

#include <vector>

class CBobResult
{
	int GetInteger(unsigned Index) const { return 0; }
	const char *GetString(unsigned Index) const { return "todo"; }
	int GetVictim() const { return 0; }
	int NumArguments() const { return 0; }
	int m_ClientId = -1;
};

typedef void (*FBobCommandCallback)(CBobResult *pResult, void *pUserData);

class CBobCommand
{
public:
	const char *m_pName;
	const char *m_pHelp;
	const char *m_pParams;
	FBobCommandCallback m_pfnFunc;
	void *m_pUser;

	CBobCommand(
		const char *pName,
		const char *pHelp,
		const char *pParams,
		FBobCommandCallback pfnFunc,
		void *pUser) :
		m_pName(pName),
		m_pHelp(pHelp),
		m_pParams(pParams),
		m_pfnFunc(pfnFunc),
		m_pUser(pUser)
	{
	}
};

class CBobConsole
{
public:
	std::vector<CBobCommand> m_vCommands;

	void Register(
		const char *pName,
		const char *pParams,
		FBobCommandCallback pfnFunc,
		void *pUser,
		const char *pHelp);

	// returns true if the command exists
	bool ExecuteCmd(const char *pCommand);
};

// TODO: add custom authorization for antibot commands something like this

// antibot dump
// antibot access_level dump 1
// antibot dump

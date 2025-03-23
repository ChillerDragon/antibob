#include "console.h"
#include "base/system.h"

void CBobConsole::Register(
	const char *pName,
	const char *pParams,
	FBobCommandCallback pfnFunc,
	void *pUser,
	const char *pHelp)
{
	m_vCommands.emplace_back(pName, pParams, pHelp, pfnFunc, pUser);
}

bool CBobConsole::ExecuteCmd(const char *pCommand)
{
	for(auto Cmd : m_vCommands)
	{
		// TODO: proper arg parsing

		if(str_startswith(pCommand, Cmd.m_pName))
		{
			CBobResult Result;
			Cmd.m_pfnFunc(&Result, Cmd.m_pUser);
			return true;
		}
	}
	return false;
}

#pragma once

#include <base/system.h>

#include <vector>

#define MAX_ARGS 10

class CBobParam
{
public:
	bool m_Optional;
	char m_aDescription[512];
	enum class EType
	{
		INT,
		STRING,
		REST,
		INVALID
	};
	EType m_Type = EType::INVALID;

	void Reset()
	{
		m_Optional = false;
		m_aDescription[0] = '\0';
		m_Type = EType::INVALID;
	}
};

class CBobResult
{
	char m_aaArgs[10][512];
	int m_NumArgs = 0;
	std::vector<CBobParam> m_vParams;
	void ParseParams();
	const char *ParamsText() { return m_aParamsText; }
	int NumParams() { return m_vParams.size(); }

public:
	int GetInteger(unsigned Index) const { return atoi(m_aaArgs[Index]); }
	const char *GetString(unsigned Index) const { return m_aaArgs[Index]; }
	// int GetVictim() const { return 0; }
	int NumArguments() const { return m_NumArgs; }
	int NumRequiredParamaters() const;
	int m_ClientId = -1;

	char m_aRawArgs[2028];
	char m_aParamsText[512];
	char m_aShortParams[512];

	CBobResult(const char *pRawArgs, const char *pParams)
	{
		str_copy(m_aRawArgs, pRawArgs);
		str_copy(m_aParamsText, pParams);
		for(auto *pArg : m_aaArgs)
			pArg[0] = '\0';

		m_vParams.clear();
		ParseParams();
	}

	bool ParseArgs(char *pError, int ErrorSize);
};

typedef void (*FBobCommandCallback)(CBobResult *pResult, void *pUserData);

class CBobCommand
{
public:
	const char *m_pName;
	const char *m_pParams;
	const char *m_pHelp;
	FBobCommandCallback m_pfnFunc;
	void *m_pUser;

	CBobCommand(
		const char *pName,
		const char *pParams,
		const char *pHelp,
		FBobCommandCallback pfnFunc,
		void *pUser) :
		m_pName(pName),
		m_pParams(pParams),
		m_pHelp(pHelp),
		m_pfnFunc(pfnFunc),
		m_pUser(pUser)
	{
	}
};

class CBobConsole
{
public:
	static bool ParseParams(std::vector<CBobParam> &vParams, const char *pParamsText);

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

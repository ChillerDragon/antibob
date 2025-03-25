#include "console.h"
#include "base/log.h"
#include "base/system.h"

int CBobResult::NumRequiredParamaters() const
{
	int Num = 0;
	for(auto Param : m_vParams)
		if(!Param.m_Optional)
			Num++;
	return Num;
}

bool CBobConsole::ParseParams(std::vector<CBobParam> &vParams, const char *pParamsText)
{
	const int ParamsLen = str_length(pParamsText);
	bool InDesc = false;
	int k = 0;
	CBobParam Param;
	Param.Reset();

	for(int i = 0; i < ParamsLen; i++)
	{
		if(pParamsText[i] == '[')
		{
			if(InDesc)
			{
				log_error("antibot", "nested braces in params '%s'", pParamsText);
				return false;
			}
			InDesc = true;
			continue;
		}
		if(pParamsText[i] == ']')
		{
			if(!InDesc)
			{
				log_error("antibot", "unexpected closing brace '%s'", pParamsText);
				return false;
			}

			InDesc = false;
			vParams.emplace_back(Param);
			Param.Reset();
			continue;
		}

		if(pParamsText[i] == '?')
		{
			if(Param.m_Optional)
			{
				log_error("antibot", "nameless optional parameter in %s", pParamsText);
				return false;
			}

			Param.m_Optional = true;
		}
		else if(!InDesc)
		{
			switch(pParamsText[i])
			{
			case 'i':
				Param.m_Type = CBobParam::EType::INT;
				break;
			case 's':
				Param.m_Type = CBobParam::EType::STRING;
				break;
			case 'r':
				Param.m_Type = CBobParam::EType::REST;
				break;
			default:
				log_error("antibot", "invalid parameter type %c in params %s", pParamsText[i], pParamsText);
				return false;
			}
			if(pParamsText[i + 1] != '[')
			{
				vParams.emplace_back(Param);
				Param.Reset();
			}
		}
	}

	if(Param.m_Optional)
	{
		log_error("antibot", "nameless optional parameter in %s", pParamsText);
		return false;
	}

	if(InDesc)
	{
		log_error("antibot", "missing ] in params '%s'", pParamsText);
		return false;
	}

	return true;
}

void CBobResult::ParseParams()
{
	m_vParams.clear();
	CBobConsole::ParseParams(m_vParams, m_aParamsText);
}

bool CBobResult::ParseArgs(char *pError, int ErrorSize)
{
	m_NumArgs = 0;
	const int LineLen = str_length(m_aRawArgs);
	int k = 0;
	bool IsRest = false;
	for(int i = 0; i < LineLen; i++)
	{
		if(m_NumArgs < m_vParams.size())
			if(m_vParams[m_NumArgs].m_Type == CBobParam::EType::REST)
				IsRest = true;

		// skip spaces between args
		if(m_aRawArgs[i] == ' ' && k == 0)
			continue;

		if(m_aRawArgs[i] == ' ' && !IsRest)
		{
			m_aaArgs[m_NumArgs][k++] = '\0';
			m_NumArgs++;
			k = 0;
			continue;
		}

		m_aaArgs[m_NumArgs][k++] = m_aRawArgs[i];
	}
	if(k)
	{
		m_aaArgs[m_NumArgs][k++] = '\0';
		m_NumArgs++;
	}

	int NumMin = NumRequiredParamaters();
	int NumMax = m_vParams.size();
	int NumGot = NumArguments();
	if(NumGot < NumMin || NumGot > NumMax)
	{
		if(pError)
		{
			if(NumMin == NumMax)
				str_format(pError, ErrorSize, "got %d out of %d arguments %s", NumGot, NumMin, ParamsText());
			else
				str_format(pError, ErrorSize, "got %d out of %d..%d arguments %s", NumGot, NumMin, NumMax, ParamsText());
		}
		return false;
	}

	return true;
}

void CBobConsole::Register(
	const char *pName,
	const char *pParams,
	FBobCommandCallback pfnFunc,
	void *pUser,
	const char *pHelp)
{
	m_vCommands.emplace_back(pName, pParams, pHelp, pfnFunc, pUser);
}

static bool MatchCmd(const char *pName, const char *pCommandWithArgs)
{
	if(!str_comp_nocase(pName, pCommandWithArgs))
		return true;

	if(!str_startswith_nocase(pCommandWithArgs, pName))
		return false;

	char CmdEnd = pCommandWithArgs[str_length(pName)];
	return CmdEnd == ' ';
}

bool CBobConsole::ExecuteCmd(const char *pCommand)
{
	for(auto Cmd : m_vCommands)
	{
		if(MatchCmd(Cmd.m_pName, pCommand))
		{
			CBobResult Result(pCommand + str_length(Cmd.m_pName), Cmd.m_pParams);

			char aError[512];
			if(!Result.ParseArgs(aError, sizeof(aError)))
			{
				log_error("antibot", "failed to parse %s arguments: %s", Cmd.m_pName, aError);
				return true;
			}

			Cmd.m_pfnFunc(&Result, Cmd.m_pUser);
			return true;
		}
	}
	return false;
}

void CBobConsole::PrintCmdHelp(const char *pCommand)
{
	for(auto Cmd : m_vCommands)
	{
		if(!str_comp_nocase(pCommand, Cmd.m_pName))
		{
			log_info("antibot", "Usage: %s %s", Cmd.m_pName, Cmd.m_pParams);
			log_info("antibot", "%s", Cmd.m_pHelp);
			return;
		}
	}
	log_info("antibot", "no such command '%s'", pCommand);
}

void CBobConsole::PrintCmdlist()
{
	char aCommands[2048];
	aCommands[0] = '\0';

	int i = 0;
	for(auto Cmd : m_vCommands)
	{
		str_append(aCommands, Cmd.m_pName);
		i++;
		if(i != m_vCommands.size())
			str_append(aCommands, ", ");
	}

	log_info("antibot", "%s", aCommands);
}

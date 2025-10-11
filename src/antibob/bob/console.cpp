#include <cstdlib>
#include <polybob/base/log.h>
#include <polybob/base/system.h>

#include <bob/antibob.h>

#include "console.h"

int CBobResult::NumRequiredParamaters() const
{
	int Num = 0;
	for(auto Param : m_vParams)
		if(!Param.m_Optional)
			Num++;
	return Num;
}

bool CBobConsole::ParseParams(std::vector<CBobParam> &vParams, const char *pParamsText, char *pError, int ErrorSize)
{
	const int ParamsLen = str_length(pParamsText);
	bool InDesc = false;
	int k = 0;
	CBobParam Param;
	Param.Reset();
	pError[0] = '\0';

	for(int i = 0; i < ParamsLen; i++)
	{
		if(pParamsText[i] == '[')
		{
			if(InDesc)
			{
				str_format(pError, ErrorSize, "nested braces in params '%s'", pParamsText);
				return false;
			}
			InDesc = true;
			continue;
		}
		if(pParamsText[i] == ']')
		{
			if(!InDesc)
			{
				str_format(pError, ErrorSize, "unexpected closing brace '%s'", pParamsText);
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
				str_format(pError, ErrorSize, "nameless optional parameter in %s", pParamsText);
				return false;
			}

			Param.m_Optional = true;
		}
		else if(InDesc)
		{
			char aDescChar[4];
			str_format(aDescChar, sizeof(aDescChar), "%c", pParamsText[i]);
			str_append(Param.m_aDescription, aDescChar);
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
				str_format(pError, ErrorSize, "invalid parameter type %c in params %s", pParamsText[i], pParamsText);
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
		str_format(pError, ErrorSize, "nameless optional parameter in %s", pParamsText);
		return false;
	}

	if(InDesc)
	{
		str_format(pError, ErrorSize, "missing ] in params '%s'", pParamsText);
		return false;
	}

	return true;
}

void CBobResult::ParseParams()
{
	m_vParams.clear();

	char aBuf[1024];
	// this can technically fail but we already checked the params
	// on server launch so it should be safe to ignore it
	CBobConsole::ParseParams(m_vParams, m_aParamsText, aBuf, sizeof(aBuf));
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

	for(int i = 0; i < m_NumArgs; i++)
	{
		switch(m_vParams[i].m_Type)
		{
		case CBobParam::EType::INT:
			int Value;
			if(!str_toint(m_aaArgs[i], &Value) ||
				Value == std::numeric_limits<int>::max() || Value == std::numeric_limits<int>::min())
			{
				str_format(pError, ErrorSize, "argument '%s' has value '%s' which is not a valid integer", m_vParams[i].m_aDescription, m_aaArgs[i]);
				return false;
			}
			break;
		case CBobParam::EType::STRING:
		case CBobParam::EType::REST:
		case CBobParam::EType::INVALID:
			break;
		}
	}

	return true;
}

void CBobConsole::OnInit(CBobConfigManager *pConfigManager, CAntibob *pAntibob)
{
	m_pConfigManager = pConfigManager;
	m_pAntibob = pAntibob;
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
				m_pAntibob->LogError("failed to parse %s arguments: %s", Cmd.m_pName, aError);
				return true;
			}

			Cmd.m_pfnFunc(&Result, Cmd.m_pUser);
			return true;
		}
	}
	return m_pConfigManager->OnConsoleCommand(pCommand, m_pAntibob);
}

void CBobConsole::PrintCmdHelp(const char *pCommand)
{
	for(auto Cmd : m_vCommands)
	{
		if(!str_comp_nocase(pCommand, Cmd.m_pName))
		{
			m_pAntibob->LogInfo("Usage: %s %s", Cmd.m_pName, Cmd.m_pParams);
			m_pAntibob->LogInfo("%s", Cmd.m_pHelp);
			return;
		}
	}
	m_pAntibob->LogInfo("no such command '%s'", pCommand);
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

	m_pAntibob->LogInfo("%s", aCommands);
}

// TODO: this is not matching the ddnet implementation!
//       whis needs unit tests and has to be corrected
//       for example "foo bar \\" baz" parses differently
bool CBobConsole::ParseStringQuotes(const char *pStringInput, char *pOut, int OutSize)
{
	// skip initial spaces
	while(*pStringInput == ' ')
		++pStringInput;
	const int InLen = str_length(pStringInput);
	bool IsQuoted = false;
	int OutIdx = 0;
	for(int i = 0; i < InLen; i++)
	{
		bool Escaped = i && pStringInput[i - 1] == '\\';
		if(pStringInput[i] == '"')
		{
			if(Escaped)
			{
				// overwrite \ with "
				OutIdx--;
			}
			else if(i == 0)
			{
				// start of string
				IsQuoted = true;
				continue;
			}
			else if(IsQuoted)
			{
				// end of string
				break;
			}
		}
		// last character of input
		if(i + 1 == InLen)
		{
			// missing closing "
			if(IsQuoted)
				return false;
		}
		if(OutIdx >= OutSize - 1)
			break;
		pOut[OutIdx++] = pStringInput[i];
	}
	pOut[OutIdx] = '\0';
	return true;
}

void CBobConsole::EscapeRconString(char *pBuf, int BufSize, const char *pStr)
{
	int BufIdx = 0;
	bool Error = false;

	auto SafeAppend = [&pBuf, BufSize, &BufIdx, &Error](char NewChar) -> bool {
		if(Error)
			return false;
		if(BufIdx >= BufSize - 1)
		{
			Error = true;
			str_copy(pBuf, "error", BufSize);
			return false;
		}
		pBuf[BufIdx++] = NewChar;
		return true;
	};

	for(int i = 0; i < str_length(pStr); i++)
	{
		if(pStr[i] == '"')
			SafeAppend('\\');
		else if(pStr[i] == '\\')
			SafeAppend('\\');
		SafeAppend(pStr[i]);
	}
	SafeAppend('\0');
}

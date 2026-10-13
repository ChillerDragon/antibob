#include "console.h"

#include <bob/antibob.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>
#include <polybob/engine/shared/protocol.h>

std::optional<std::vector<int>> CBobConsole::ClientsForVictimFallback(int ClientId, const char *pVictim, void *pUser)
{
	CBobConsole *pSelf = (CBobConsole *)pUser;
	std::vector<int> vClientIds;

	log_warn("antibot", "WARNING: ClientsForVictimFallback() called. This should only happen in unit tests!");

	if(!str_comp(pVictim, "me"))
	{
		if(ClientId < 0)
			return std::nullopt;
		vClientIds.emplace_back(ClientId);
	}
	else if(!str_comp(pVictim, "all"))
	{
		// this is wrong but we are just a stupid fallback
		vClientIds.emplace_back(ClientId);
	}
	else
	{
		return std::nullopt;
	}

	return std::make_optional(std::move(vClientIds));
}

int CBobResult::GetVictim(unsigned Slot) const
{
	dbg_assert(Slot < m_vVictims.size(), "victim slot %u out of range", Slot);
	dbg_assert(m_vVictims[Slot].m_Id.has_value(), "victim %u has no value", Slot);
	return m_vVictims[Slot].m_Id.value();
}

void CBobResult::SetVictim(unsigned Slot, int Victim)
{
	dbg_assert(Slot < m_vVictims.size(), "victim slot %u out of range", Slot);
	dbg_assert(in_range(Victim, 0, MAX_CLIENTS - 1), "Victim ID %d out of range [0, %d]", Victim, MAX_CLIENTS - 1);
	m_vVictims[Slot].m_Id = Victim;
}

void CBobResult::AddVictim(const char *pVictim)
{
	CVictim Victim;
	int Value;
	if(str_toint(pVictim, &Value) && in_range(Value, 0, MAX_CLIENTS - 1))
		Victim.m_Id = Value;
	else
		str_copy(Victim.m_aSpecialVictim, pVictim);
	m_vVictims.push_back(Victim);
}

int CBobResult::NumRequiredParameters() const
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
	CBobParam Param;
	Param.Reset();
	pError[0] = '\0';

	// as soon as the first optional param was defined
	// nothing after it can be non optional
	bool ExpectOnlyOptionals = false;

	for(int i = 0; i < ParamsLen; i++)
	{
		// allow spaces between params
		if(!InDesc && pParamsText[i] == ' ')
			continue;

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

			if(Param.m_Type == CBobParam::EType::INVALID)
			{
				str_format(pError, ErrorSize, "got invalid type for param in '%s'", pParamsText);
				return false;
			}
			if(ExpectOnlyOptionals && !Param.m_Optional)
			{
				str_format(pError, ErrorSize, "got non optional param after optional param in '%s'", pParamsText);
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
				str_format(pError, ErrorSize, "nameless optional parameter in '%s'", pParamsText);
				return false;
			}
			if(pParamsText[i + 1] == '[')
			{
				str_format(pError, ErrorSize, "missing type for optional parameter in %s", pParamsText);
				return false;
			}

			Param.m_Optional = true;
			ExpectOnlyOptionals = true;
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
			case 'v':
				Param.m_Type = CBobParam::EType::VICTIM;
				break;
			default:
				str_format(pError, ErrorSize, "invalid parameter type %c in params %s", pParamsText[i], pParamsText);
				return false;
			}
			if(pParamsText[i + 1] != '[')
			{
				if(Param.m_Type == CBobParam::EType::INVALID)
				{
					str_format(pError, ErrorSize, "got invalid type for param in '%s'", pParamsText);
					return false;
				}
				if(ExpectOnlyOptionals && !Param.m_Optional)
				{
					str_format(pError, ErrorSize, "got non optional param after optional param in '%s'", pParamsText);
					return false;
				}
				vParams.emplace_back(Param);
				Param.Reset();
			}
		}
	}

	if(Param.m_Optional)
	{
		str_format(pError, ErrorSize, "nameless optional parameter in '%s'", pParamsText);
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
	if(pError && ErrorSize)
		pError[0] = '\0';
	const int LineLen = str_length(m_aRawArgs);
	int k = 0;
	bool IsRest = false;
	bool IsQuoted = false;
	for(int i = 0; i < LineLen; i++)
	{
		if(m_NumArgs < m_vParams.size())
			if(m_vParams[m_NumArgs].m_Type == CBobParam::EType::REST)
				IsRest = true;

		bool Escaped = i && m_aRawArgs[i - 1] == '\\';
		if(m_aRawArgs[i] == '"' && !IsRest)
		{
			if(Escaped)
			{
				// overwrite \ with "
				k = std::max(0, k - 1);
			}
			else if(k == 0)
			{
				// start of string
				IsQuoted = true;
				continue;
			}
			else if(IsQuoted)
			{
				// end of string
				IsQuoted = false;
				m_aaArgs[m_NumArgs][k++] = '\0';
				m_NumArgs++;
				k = 0;
				continue;
			}
		}
		// last character of input
		if(i + 1 == LineLen)
		{
			// missing closing "
			if(IsQuoted)
			{
				if(pError)
				{
					str_copy(pError, "Expected closing quote (\") but got end of input", ErrorSize);
				}
				return false;
			}
		}

		// skip spaces between args
		if(m_aRawArgs[i] == ' ' && k == 0)
			continue;

		if(m_aRawArgs[i] == ' ' && !IsRest && !IsQuoted)
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

	int NumMin = NumRequiredParameters();
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
		char aParamName[512];
		if(m_vParams[i].m_aDescription[0])
		{
			str_copy(aParamName, m_vParams[i].m_aDescription);
		}
		else
		{
			str_format(
				aParamName,
				sizeof(aParamName),
				"at offset %d",
				i + 1);
		}

		int Value;
		switch(m_vParams[i].m_Type)
		{
		case CBobParam::EType::INT:
			if(!str_toint(m_aaArgs[i], &Value) ||
				Value == std::numeric_limits<int>::max() || Value == std::numeric_limits<int>::min())
			{
				str_format(pError, ErrorSize, "argument '%s' has value '%s' which is not a valid integer", aParamName, m_aaArgs[i]);
				return false;
			}
			break;
		case CBobParam::EType::VICTIM:
			if(!str_toint(m_aaArgs[i], &Value) ||
				Value == std::numeric_limits<int>::max() || Value == std::numeric_limits<int>::min())
			{
				if(str_comp(m_aaArgs[i], "all") == 0)
				{
					AddVictim(m_aaArgs[i]);
					continue;
				}
				if(str_comp(m_aaArgs[i], "me") == 0)
				{
					// depends on https://github.com/ddnet/ddnet/pull/11688
					str_format(pError, ErrorSize, "argument '%s' has value '%s' which is not yet supported for victim", aParamName, m_aaArgs[i]);
					return false;
				}
				str_format(pError, ErrorSize, "argument '%s' has value '%s' which is not a valid victim", aParamName, m_aaArgs[i]);
				return false;
			}
			if(Value < 0)
			{
				str_format(pError, ErrorSize, "argument '%s' has value %d which is not a valid victim (to small)", aParamName, Value);
				return false;
			}
			if(Value >= MAX_CLIENTS)
			{
				str_format(pError, ErrorSize, "argument '%s' has value %d which is not a valid victim (to big)", aParamName, Value);
				return false;
			}
			AddVictim(m_aaArgs[i]);
			break;
		case CBobParam::EType::STRING:
		case CBobParam::EType::REST:
		case CBobParam::EType::INVALID:
			break;
		}
	}

	// TODO: this code is untested there might be off by one errors
	for(int i = m_NumArgs; i < m_vParams.size(); i++)
	{
		if(m_vParams[i].m_Optional && m_vParams[i].m_Type == CBobParam::EType::VICTIM)
		{
			// If the victim argument is optional we default to "me"
			AddVictim("me");
		}
	};

	return true;
}

CBobConsole::CBobConsole()
{
	SetGetVictimsCommandCallback(ClientsForVictimFallback, this);
}

void CBobConsole::OnInit(CBobConfigManager *pConfigManager)
{
	m_pConfigManager = pConfigManager;
}

void CBobConsole::SetGetVictimsCommandCallback(FGetVictimsCommandCallback pfnCallback, void *pUser)
{
	m_pfnGetVictimsCommandCallback = pfnCallback;
	m_pGetVictimsCommandUserData = pUser;
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

bool CBobConsole::ExecuteCmd(int ClientId, const char *pCommand)
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

			int FanOutSlot = -1;
			std::vector<int> vFanOutIds;
			for(unsigned Slot = 0; Slot < Result.m_vVictims.size(); Slot++)
			{
				if(!Result.m_vVictims[Slot].m_aSpecialVictim[0])
					continue;
				std::optional<std::vector<int>> Victims;
				if(m_pfnGetVictimsCommandCallback)
					Victims = m_pfnGetVictimsCommandCallback(ClientId, Result.m_vVictims[Slot].m_aSpecialVictim, m_pGetVictimsCommandUserData);

				if(!Victims.has_value())
				{
					log_error("antibot", "Invalid victim '%s'", Result.m_vVictims[Slot].m_aSpecialVictim);
					return true;
				}
				if(Victims->size() != 1 && FanOutSlot >= 0)
				{
					log_error("antibot", "Only one parameter may target multiple clients");
					return true;
				}
				if(Victims->size() == 1)
					Result.SetVictim(Slot, Victims->front());
				else
				{
					FanOutSlot = Slot;
					vFanOutIds = std::move(Victims.value());
				}
			}

			if(FanOutSlot < 0)
			{
				Cmd.m_pfnFunc(&Result, Cmd.m_pUser);
			}
			else
			{
				for(const int VictimId : vFanOutIds)
				{
					Result.SetVictim(FanOutSlot, VictimId);
					Cmd.m_pfnFunc(&Result, Cmd.m_pUser);
				}
			}

			return true;
		}
	}
	return m_pConfigManager->OnConsoleCommand(pCommand);
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

	if(m_pConfigManager->PrintHelp(pCommand))
		return;

	log_info("antibot", "no such command '%s'", pCommand);
}

void CBobConsole::PrintConfigs()
{
	m_pConfigManager->PrintConfigs();
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

// TODO: this is not matching the ddnet implementation!
//       which needs unit tests and has to be corrected
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

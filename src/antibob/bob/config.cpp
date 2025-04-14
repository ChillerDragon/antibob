#include <polybob/base/log.h>
#include <polybob/base/system.h>

#include <bob/console.h>

#include <algorithm>

#include "config.h"

CBobConfig g_BobConfig;

CBobStringConfigVariable::CBobStringConfigVariable(const char *pScriptName,
	EVarType Type,
	int Flags,
	const char *pHelp,
	char *pStr,
	const char *pDefault,
	size_t MaxSize) :
	CBobConfigVariable(pScriptName, Type, Flags, pHelp),
	m_pStr(pStr),
	m_pDefault(pDefault),
	m_MaxSize(MaxSize)
{
	str_copy(m_pStr, m_pDefault, m_MaxSize);
}

void CBobConfigManager::OnInit()
{
	m_vpAllVariables.clear();

#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Flags, Desc) \
	m_vpAllVariables.emplace_back(new CBobIntConfigVariable(#ScriptName, CBobConfigVariable::EVarType::INT, Flags, Desc, &g_BobConfig.m_##Name, Def, Min, Max));
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Flags, Desc) \
	m_vpAllVariables.emplace_back(new CBobStringConfigVariable(#ScriptName, CBobConfigVariable::EVarType::STRING, Flags, Desc, g_BobConfig.m_##Name, Def, Len));
#include <bob/config_variables.h>
#undef MACRO_CONFIG_INT
#undef MACRO_CONFIG_STR
}

bool CBobConfigManager::OnConsoleCommand(const char *pCommand)
{
	for(auto *pVariable : m_vpAllVariables)
	{
		if(!str_startswith(pCommand, pVariable->m_pScriptName))
			continue;
		// Detect substrings. For example ab_kick_all should not match ab_kick
		const char *pEnd = pCommand + str_length(pVariable->m_pScriptName);
		if(*pEnd != ' ' && *pEnd != '\0')
			continue;
		// seek away spaces so "antibot ab_kick_reason   noob"
		// sets the string "noob" not "  noob"
		// and "antibot ab_kick_reason "
		// prints the reason instead of setting it to ""
		while(*pEnd == ' ')
			++pEnd;

		if(*pEnd == '\0')
		{
			char aValue[2048];
			switch(pVariable->m_Type)
			{
			case CBobConfigVariable::EVarType::INT:
				str_format_int(aValue, sizeof(aValue), *((CBobIntConfigVariable *)pVariable)->m_pVariable);
				break;
			case CBobConfigVariable::EVarType::STRING:
				str_copy(aValue, ((CBobStringConfigVariable *)pVariable)->m_pStr);
				break;
			}
			log_info("antibot", "config %s value: %s", pVariable->m_pScriptName, aValue);
			return true;
		}

		CBobStringConfigVariable *pStr = nullptr;
		CBobIntConfigVariable *pInt = nullptr;

		switch(pVariable->m_Type)
		{
		case CBobConfigVariable::EVarType::INT:
			pInt = (CBobIntConfigVariable *)pVariable;
			if(!str_toint(pEnd, pInt->m_pVariable))
			{
				log_info("antibot", "'%s' is not a valid integer", pEnd);
			}
			*pInt->m_pVariable = std::clamp(*pInt->m_pVariable, pInt->m_Min, pInt->m_Max);
			break;
		case CBobConfigVariable::EVarType::STRING:
			pStr = (CBobStringConfigVariable *)pVariable;
			if(!CBobConsole::ParseStringQuotes(pEnd, pStr->m_pStr, pStr->m_MaxSize))
			{
				log_info("antibot", "failed to parse string");
			}
			break;
		}

		return true;
	}
	return false;
}

CBobConfigManager::~CBobConfigManager()
{
	for(auto *pVariable : m_vpAllVariables)
		delete pVariable;
}

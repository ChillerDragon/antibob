#pragma once

#include <cstddef>
#include <vector>

class CBobConfig
{
public:
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Flags, Desc) \
	static constexpr int ms_##Name = Def; \
	int m_##Name = Def;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Flags, Desc) \
	static constexpr unsigned ms_##Name = Def; \
	unsigned m_##Name = Def;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Flags, Desc) \
	static constexpr const char *ms_p##Name = Def; \
	char m_##Name[Len]; // Flawfinder: ignore
#include <bob/config_variables.h>
#undef MACRO_CONFIG_INT
#undef MACRO_CONFIG_COL
#undef MACRO_CONFIG_STR
};

#define g_Config g_Config_IS_NOT_SUPPORTED_USE_g_BobConfig_INSTEAD
extern CBobConfig g_BobConfig;

class CBobConfigVariable
{
public:
	virtual ~CBobConfigVariable() = default;

	enum class EVarType
	{
		INT,
		STRING,
	};
	EVarType m_Type;
	const char *m_pScriptName;
	int m_Flags;
	const char *m_pHelp;
	CBobConfigVariable(const char *pScriptName, EVarType Type, int Flags, const char *pHelp) :
		m_pScriptName(pScriptName),
		m_Type(Type),
		m_Flags(Flags),
		m_pHelp(pHelp) {}
};

class CBobIntConfigVariable : public CBobConfigVariable
{
public:
	int *m_pVariable;
	int m_Default;
	int m_Min;
	int m_Max;

	CBobIntConfigVariable(
		const char *pScriptName,
		EVarType Type,
		int Flags,
		const char *pHelp,
		int *pVariable,
		int Default,
		int Min,
		int Max) :
		CBobConfigVariable(pScriptName, Type, Flags, pHelp),
		m_pVariable(pVariable),
		m_Default(Default),
		m_Min(Min),
		m_Max(Max)
	{
		*m_pVariable = m_Default;
	}
};

class CBobStringConfigVariable : public CBobConfigVariable
{
public:
	char *m_pStr;
	const char *m_pDefault;
	size_t m_MaxSize;

	CBobStringConfigVariable(const char *pScriptName,
		EVarType Type,
		int Flags,
		const char *pHelp,
		char *pStr,
		const char *pDefault,
		size_t MaxSize);
};

class CBobConfigManager
{
public:
	std::vector<CBobConfigVariable *> m_vpAllVariables;
	void OnInit();
	// returns true if the console command operated on a config
	bool OnConsoleCommand(const char *pCommand, class CAntibob *pAntibob);
	~CBobConfigManager();
};

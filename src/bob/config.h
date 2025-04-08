#pragma once

class CBobConfig
{
public:
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Flags, Desc) \
	static constexpr int ms_##Name = Def; \
	int m_##Name;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Flags, Desc) \
	static constexpr unsigned ms_##Name = Def; \
	unsigned m_##Name;
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

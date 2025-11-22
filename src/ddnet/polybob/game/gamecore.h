#pragma once

#include <polybob/engine/shared/protocol.h>

namespace polybob {

class CTuneParam
{
	int m_Value;

public:
	void Set(int v) { m_Value = v; }
	int Get() const { return m_Value; }
	CTuneParam &operator=(int v)
	{
		m_Value = (int)(v * 100.0f);
		return *this;
	}
	CTuneParam &operator=(float v)
	{
		m_Value = (int)(v * 100.0f);
		return *this;
	}
	operator float() const { return m_Value / 100.0f; }
};

class CTuningParams
{
	static const char *ms_apNames[];

public:
	CTuningParams()
	{
#define MACRO_TUNING_PARAM(Name, ScriptName, Value, Description) m_##Name.Set((int)((Value) * 100.0f));
#include "tuning.h"
#undef MACRO_TUNING_PARAM
	}

#define MACRO_TUNING_PARAM(Name, ScriptName, Value, Description) CTuneParam m_##Name;
#include "tuning.h"
#undef MACRO_TUNING_PARAM

	static int Num()
	{
		return sizeof(CTuningParams) / sizeof(int);
	}
	bool Set(int Index, float Value);
	bool Set(const char *pName, float Value);
	bool Get(int Index, float *pValue) const;
	bool Get(const char *pName, float *pValue) const;
	static const char *Name(int Index) { return ms_apNames[Index]; }
	float GetWeaponFireDelay(int Weapon) const;
};

} // namespace polybob

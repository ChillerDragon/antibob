#include "antibob_abi.h"

#include <polybob/base/log.h>
#include <polybob/base/system.h>

#if defined(CONF_FAMILY_WINDOWS)
#include <libloaderapi.h>
#define LOOKUP_SYM(name) \
	m_pfn##name = (F##name)GetProcAddress(GetModuleHandleA(NULL), "Antibob" #name); \
	if(!m_pfn##name) \
		log_error("antibot", "symbol 'Antibob" #name "' not found");
#else
#include <dlfcn.h>
#define LOOKUP_SYM(name) \
	m_pfn##name = (F##name)dlsym(RTLD_DEFAULT, "Antibob" #name); \
	if(!m_pfn##name) \
		log_error("antibot", "symbol 'Antibob" #name "' not found");
#endif

void CAntibobAbi::OnInit()
{
	LOOKUP_SYM(Version);
	LOOKUP_SYM(Rcon);

	if(ServerAbiVersion() != OwnAbiVersion())
	{
		log_error("antibot", "antibob abi version mismatch server=%d antibob=%d", ServerAbiVersion(), OwnAbiVersion());
	}
}

#undef LOOKUP_SYM

int CAntibobAbi::ServerAbiVersion()
{
	if(!m_pfnVersion)
		return 0;
	return m_pfnVersion();
}

bool CAntibobAbi::Rcon(const char *pLine)
{
	if(!m_pfnRcon)
		return false;
	m_pfnRcon(pLine);
	return true;
}

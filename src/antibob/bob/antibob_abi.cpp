#include "antibob_abi.h"

#include <dlfcn.h>
#include <polybob/base/log.h>
#include <polybob/base/system.h>

#define LOOKUP_SYM(name) \
	m_pfn##name = (F##name)dlsym(RTLD_DEFAULT, "Antibob" #name); \
	if(!m_pfn##name) \
		log_error("antibot", "symbol 'Antibob" #name "' not found");

void CAntibobAbi::OnInit()
{
	LOOKUP_SYM(Version);
	LOOKUP_SYM(Rcon);

	if(ServerAbiVersion() != OwnAbiVersion())
	{
		log_error("antibot", "antibob abi version missmatch server=%d antibob=%d", ServerAbiVersion(), OwnAbiVersion());
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

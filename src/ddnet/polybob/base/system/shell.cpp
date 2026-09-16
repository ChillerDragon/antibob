#include <polybob/base/detect.h>
#include <polybob/base/system/shell.h>

#if defined(CONF_FAMILY_WINDOWS)
#include <process.h>
#else
#include <unistd.h>
#endif

namespace polybob
{

	int pid()
	{
#if defined(CONF_FAMILY_WINDOWS)
		return _getpid();
#else
		return getpid();
#endif
	}

} // namespace polybob

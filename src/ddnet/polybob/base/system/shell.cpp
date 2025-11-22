#include <polybob/base/detect.h>
#include <polybob/base/system/shell.h>
#include <unistd.h>

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

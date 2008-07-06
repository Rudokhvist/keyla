#include "../common.h"
#include "FatalError.h"

/* static */ tstring FatalError::lastWin32Error() {
	LPTSTR buffer = 0;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				0, GetLastError(), 0, reinterpret_cast<LPTSTR>(&buffer), 0, 0);
	tstring ret = buffer;
	LocalFree(static_cast<HLOCAL>(buffer));
	return ret;
}

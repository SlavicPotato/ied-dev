#include "pch.h"

#include "Logging.h"

#include <ext/IOS.h>

namespace IED
{
	namespace Util
	{
		namespace Logging
		{
			void AbortPopupWrite(const char* a_message)
			{
				gLog.FatalError("%s", a_message);
				WinApi::MessageBoxError(PLUGIN_NAME, a_message);
				std::_Exit(1);
			}

			void AbortPopup(const char* a_message)
			{
				WinApi::MessageBoxErrorLog(PLUGIN_NAME, a_message);
				std::_Exit(1);
			}
		}  // namespace Logging
	}      // namespace Util
}  // namespace IED
#pragma once

#include "UIExtraLightEditorWidgetStrings.h"

namespace IED
{
	namespace Data
	{
		struct configExtraLight_t;
		struct extraLightData_t;
	}

	namespace UI
	{
		class UIExtraLightEditorWidget
		{
			using update_func_t = std::function<void()>;

		public:
			static bool DrawExtraLightEditor(
				Data::configExtraLight_t& a_data);

		private:
			static bool DrawImpl(
				Data::extraLightData_t& a_data);
		};
	}
}

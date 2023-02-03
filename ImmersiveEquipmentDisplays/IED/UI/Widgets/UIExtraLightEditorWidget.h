#pragma once

#include "UIExtraLightEditorWidgetStrings.h"

namespace IED
{
	namespace Data
	{
		struct configExtraLight_t;
		struct ExtraLightData;
	}

	namespace UI
	{
		class UIExtraLightEditorWidget
		{
		public:
			static bool DrawExtraLightEditor(
				Data::configExtraLight_t& a_data);

		private:
			static bool DrawImpl(
				Data::ExtraLightData& a_data);
		};
	}
}

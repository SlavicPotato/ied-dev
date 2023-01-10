#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include "UIDayOfWeekSelectorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		class UIDayOfWeekSelectorWidget
		{
		public:
			static bool DrawDayOfWeekSelectorWidget(
				RE::Calendar::Day& a_type);

			static const char* day_of_week_to_desc(
				RE::Calendar::Day a_day);
		};
	}

}

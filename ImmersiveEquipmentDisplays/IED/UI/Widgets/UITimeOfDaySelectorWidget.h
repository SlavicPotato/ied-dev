#pragma once

#include "IED/TimeOfDay.h"

#include "UITimeOfDaySelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UITimeOfDaySelectorWidget
		{
		public:
			UITimeOfDaySelectorWidget() = default;

			bool DrawTimeOfDaySelector(Data::TimeOfDay& a_tod);

			const char* time_of_day_to_desc(stl::flag<Data::TimeOfDay> a_tod) const;

		private:
			mutable std::string m_buf;
		};
	}

}

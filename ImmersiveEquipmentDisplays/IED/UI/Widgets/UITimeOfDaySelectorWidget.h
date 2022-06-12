#pragma once

#include "IED/TimeOfDay.h"

#include "UITimeOfDaySelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UITimeOfDaySelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UITimeOfDaySelectorWidget(Localization::ILocalization& a_localization);

			bool DrawTimeOfDaySelector(Data::TimeOfDay& a_tod);

			const char* time_of_day_to_desc(stl::flag<Data::TimeOfDay> a_tod) const;

		private:
			using data_type = std::array<
				std::pair<
					Data::TimeOfDay,
					UITimeOfDaySelectorWidgetStrings>,
				4>;

			mutable std::string m_buf;

			static data_type m_data;
		};
	}

}

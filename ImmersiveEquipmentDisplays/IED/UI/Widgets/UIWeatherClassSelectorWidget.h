#pragma once

#include "UIWeatherClassSelectorWidgetStrings.h"

#include "IED/WeatherClassificationFlags.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIWeatherClassSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIWeatherClassSelectorWidget(
				Localization::ILocalization& a_localization);

			bool DrawWeatherClassSelector(
				WeatherClassificationFlags& a_type);

		protected:
			const char* weather_class_to_desc(stl::flag<WeatherClassificationFlags> a_type);

		private:
			using data_type = std::array<
				std::pair<
					WeatherClassificationFlags,
					UIWeatherClassSelectorWidgetStrings>,
				4>;

			std::string m_buf;

			static const data_type m_data;
		};
	}

}

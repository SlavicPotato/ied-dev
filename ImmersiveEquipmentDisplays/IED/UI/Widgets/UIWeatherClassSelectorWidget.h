#pragma once

#include "UIWeatherClassSelectorWidgetStrings.h"

#include "IED/WeatherClassificationFlags.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIWeatherClassSelectorWidget
		{
		public:
			UIWeatherClassSelectorWidget() = default;

			bool DrawWeatherClassSelector(
				WeatherClassificationFlags& a_type);

			const char* weather_class_to_desc(
				stl::flag<WeatherClassificationFlags> a_type) const;

		private:
			mutable std::string m_buf;
		};
	}

}

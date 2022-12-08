#include "pch.h"

#include "UIWeatherClassSelectorWidget.h"

#include "UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array(

			std::make_pair(WeatherClassificationFlags::kPleasant, UIWeatherClassSelectorWidgetStrings::Pleasant),
			std::make_pair(WeatherClassificationFlags::kCloudy, UIWeatherClassSelectorWidgetStrings::Cloudy),
			std::make_pair(WeatherClassificationFlags::kRainy, UIWeatherClassSelectorWidgetStrings::Rainy),
			std::make_pair(WeatherClassificationFlags::kSnow, UIWeatherClassSelectorWidgetStrings::Snow)

		);

		bool UIWeatherClassSelectorWidget::DrawWeatherClassSelector(WeatherClassificationFlags& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(UIWidgetCommonStrings::WeatherClass, "wc_type_sel"),
					weather_class_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				ImGui::PushID("1");

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::All, "1"),
						stl::underlying(std::addressof(a_type)),
						stl::underlying(WeatherClassificationFlags::kAll)))
				{
					result = true;
				}

				ImGui::PopID();

				ImGui::Separator();

				ImGui::PushID("2");

				for (auto& [i, e] : s_data)
				{
					ImGui::PushID(stl::underlying(i));

					if (ImGui::CheckboxFlagsT(
							UIL::LS<UIWeatherClassSelectorWidgetStrings, 3>(e, "1"),
							stl::underlying(std::addressof(a_type)),
							stl::underlying(i)))
					{
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::PopID();

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIWeatherClassSelectorWidget::weather_class_to_desc(
			stl::flag<WeatherClassificationFlags> a_type) const
		{
			if (a_type.test(WeatherClassificationFlags::kAll))
			{
				return UIL::LS(CommonStrings::Any);
			}
			else if (!a_type.test_any(WeatherClassificationFlags::kAll))
			{
				return UIL::LS(CommonStrings::None);
			}
			else
			{
				m_buf.clear();

				if (a_type.test(WeatherClassificationFlags::kPleasant))
				{
					m_buf += UIL::LS(UIWeatherClassSelectorWidgetStrings::Pleasant);
				}

				if (a_type.test(WeatherClassificationFlags::kCloudy))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += UIL::LS(UIWeatherClassSelectorWidgetStrings::Cloudy);
				}

				if (a_type.test(WeatherClassificationFlags::kRainy))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += UIL::LS(UIWeatherClassSelectorWidgetStrings::Rainy);
				}

				if (a_type.test(WeatherClassificationFlags::kSnow))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += UIL::LS(UIWeatherClassSelectorWidgetStrings::Snow);
				}

				return m_buf.c_str();
			}
		}
	}
}
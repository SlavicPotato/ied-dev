#include "pch.h"

#include "UIWeatherClassSelectorWidget.h"

#include "UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		const UIWeatherClassSelectorWidget::data_type UIWeatherClassSelectorWidget::m_data{ {

			{ WeatherClassificationFlags::kPleasant, UIWeatherClassSelectorWidgetStrings::Pleasant },
			{ WeatherClassificationFlags::kCloudy, UIWeatherClassSelectorWidgetStrings::Cloudy },
			{ WeatherClassificationFlags::kRainy, UIWeatherClassSelectorWidgetStrings::Rainy },
			{ WeatherClassificationFlags::kSnow, UIWeatherClassSelectorWidgetStrings::Snow },

		} };

		UIWeatherClassSelectorWidget::UIWeatherClassSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIWeatherClassSelectorWidget::DrawWeatherClassSelector(WeatherClassificationFlags& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(UIWidgetCommonStrings::WeatherClass, "wc_type_sel"),
					weather_class_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				ImGui::PushID("1");

				if (ImGui::CheckboxFlagsT(
						LS(CommonStrings::All, "1"),
						stl::underlying(std::addressof(a_type)),
						stl::underlying(WeatherClassificationFlags::kAll)))
				{
					result = true;
				}

				ImGui::PopID();

				ImGui::Separator();

				ImGui::PushID("2");

				for (auto& [i, e] : m_data)
				{
					ImGui::PushID(stl::underlying(i));

					bool selected = (i == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::CheckboxFlagsT(
							LS<UIWeatherClassSelectorWidgetStrings, 3>(e, "1"),
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
			stl::flag<WeatherClassificationFlags> a_type)
		{
			if (a_type.test(WeatherClassificationFlags::kAll))
			{
				return LS(CommonStrings::Any);
			}
			else if (!a_type.test_any(WeatherClassificationFlags::kAll))
			{
				return LS(CommonStrings::None);
			}
			else
			{
				m_buf.clear();

				if (a_type.test(WeatherClassificationFlags::kPleasant))
				{
					m_buf = LS(UIWeatherClassSelectorWidgetStrings::Pleasant);
				}

				if (a_type.test(WeatherClassificationFlags::kCloudy))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += LS(UIWeatherClassSelectorWidgetStrings::Cloudy);
				}

				if (a_type.test(WeatherClassificationFlags::kRainy))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += LS(UIWeatherClassSelectorWidgetStrings::Rainy);
				}

				if (a_type.test(WeatherClassificationFlags::kSnow))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += LS(UIWeatherClassSelectorWidgetStrings::Snow);
				}

				return m_buf.c_str();
			}
		}
	}
}
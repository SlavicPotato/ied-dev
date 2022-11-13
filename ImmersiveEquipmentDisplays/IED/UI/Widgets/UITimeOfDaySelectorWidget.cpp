#include "pch.h"

#include "UITimeOfDaySelectorWidget.h"

namespace IED
{
	namespace UI
	{
		static constexpr auto s_data = stl::make_array(

			std::make_pair(Data::TimeOfDay::kSunrise, UITimeOfDaySelectorWidgetStrings::Sunrise),
			std::make_pair(Data::TimeOfDay::kDay, UITimeOfDaySelectorWidgetStrings::Day),
			std::make_pair(Data::TimeOfDay::kSunset, UITimeOfDaySelectorWidgetStrings::Sunset),
			std::make_pair(Data::TimeOfDay::kNight, UITimeOfDaySelectorWidgetStrings::Night)

		);

		UITimeOfDaySelectorWidget::UITimeOfDaySelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UITimeOfDaySelectorWidget::DrawTimeOfDaySelector(
			Data::TimeOfDay& a_tod)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					"##ex_tod_sel",
					time_of_day_to_desc(a_tod),
					ImGuiComboFlags_HeightLarge))
			{
				ImGui::PushID("1");

				if (ImGui::CheckboxFlagsT(
						LS(CommonStrings::All, "1"),
						stl::underlying(std::addressof(a_tod)),
						stl::underlying(Data::TimeOfDay::kAll)))
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
							LS<UITimeOfDaySelectorWidgetStrings, 3>(e, "1"),
							stl::underlying(std::addressof(a_tod)),
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

		const char* UITimeOfDaySelectorWidget::time_of_day_to_desc(
			stl::flag<Data::TimeOfDay> a_type) const
		{
			if (a_type.test(Data::TimeOfDay::kAll))
			{
				return LS(CommonStrings::Any);
			}
			else if (!a_type.test_any(Data::TimeOfDay::kAll))
			{
				return LS(CommonStrings::None);
			}
			else
			{
				m_buf.clear();

				if (a_type.test(Data::TimeOfDay::kSunrise))
				{
					m_buf += LS(UITimeOfDaySelectorWidgetStrings::Sunrise);
				}

				if (a_type.test(Data::TimeOfDay::kDay))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += LS(UITimeOfDaySelectorWidgetStrings::Day);
				}

				if (a_type.test(Data::TimeOfDay::kSunset))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += LS(UITimeOfDaySelectorWidgetStrings::Sunset);
				}

				if (a_type.test(Data::TimeOfDay::kNight))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += LS(UITimeOfDaySelectorWidgetStrings::Night);
				}

				return m_buf.c_str();
			}
		}
	}
}
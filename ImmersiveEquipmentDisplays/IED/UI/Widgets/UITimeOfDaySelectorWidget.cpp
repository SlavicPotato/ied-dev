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

		bool UITimeOfDaySelectorWidget::DrawTimeOfDaySelector(
			Data::TimeOfDay& a_tod)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Flags, "ex_tod_sel"),
					time_of_day_to_desc(a_tod),
					ImGuiComboFlags_HeightLarge))
			{
				ImGui::PushID("1");

				if (ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::Any, "1"),
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
							UIL::LS<UITimeOfDaySelectorWidgetStrings, 3>(e, "1"),
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
				return UIL::LS(CommonStrings::Any);
			}
			else if (!a_type.test_any(Data::TimeOfDay::kAll))
			{
				return UIL::LS(CommonStrings::None);
			}
			else
			{
				m_buf.clear();

				if (a_type.test(Data::TimeOfDay::kSunrise))
				{
					m_buf += UIL::L(UITimeOfDaySelectorWidgetStrings::Sunrise);
				}

				if (a_type.test(Data::TimeOfDay::kDay))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += UIL::L(UITimeOfDaySelectorWidgetStrings::Day);
				}

				if (a_type.test(Data::TimeOfDay::kSunset))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += UIL::L(UITimeOfDaySelectorWidgetStrings::Sunset);
				}

				if (a_type.test(Data::TimeOfDay::kNight))
				{
					if (!m_buf.empty())
					{
						m_buf += ", ";
					}
					m_buf += UIL::L(UITimeOfDaySelectorWidgetStrings::Night);
				}

				return m_buf.c_str();
			}
		}
	}
}
#include "pch.h"

#include "UITimeOfDaySelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UITimeOfDaySelectorWidget::data_type UITimeOfDaySelectorWidget::m_data{ {

			{ Data::TimeOfDay::kSunrise, UITimeOfDaySelectorWidgetStrings::Sunrise },
			{ Data::TimeOfDay::kDay, UITimeOfDaySelectorWidgetStrings::Day },
			{ Data::TimeOfDay::kSunset, UITimeOfDaySelectorWidgetStrings::Sunset },
			{ Data::TimeOfDay::kNight, UITimeOfDaySelectorWidgetStrings::Night }

		} };

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
				for (auto& [i, e] : m_data)
				{
					ImGui::PushID(stl::underlying(i));

					bool selected = (i == a_tod);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LS<UITimeOfDaySelectorWidgetStrings, 3>(e, "1"),
							selected))
					{
						a_tod  = i;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UITimeOfDaySelectorWidget::time_of_day_to_desc(
			Data::TimeOfDay a_tod) const
		{
			switch (a_tod)
			{
			case Data::TimeOfDay::kDay:
				return LS(UITimeOfDaySelectorWidgetStrings::Day);
			case Data::TimeOfDay::kSunset:
				return LS(UITimeOfDaySelectorWidgetStrings::Sunset);
			case Data::TimeOfDay::kNight:
				return LS(UITimeOfDaySelectorWidgetStrings::Night);
			case Data::TimeOfDay::kSunrise:
				return LS(UITimeOfDaySelectorWidgetStrings::Sunrise);
			default:
				return nullptr;
			}
		}
	}
}
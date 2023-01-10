#include "pch.h"

#include "UIDayOfWeekSelectorWidget.h"

namespace IED
{
	namespace UI
	{

		static constexpr auto s_data = stl::make_array(

			std::make_pair(RE::Calendar::Day::kSundas, UIDayOfWeekSelectorWidgetStrings::Sundas),
			std::make_pair(RE::Calendar::Day::kMorndas, UIDayOfWeekSelectorWidgetStrings::Morndas),
			std::make_pair(RE::Calendar::Day::kTirdas, UIDayOfWeekSelectorWidgetStrings::Tirdas),
			std::make_pair(RE::Calendar::Day::kMiddas, UIDayOfWeekSelectorWidgetStrings::Middas),
			std::make_pair(RE::Calendar::Day::kTurdas, UIDayOfWeekSelectorWidgetStrings::Turdas),
			std::make_pair(RE::Calendar::Day::kFredas, UIDayOfWeekSelectorWidgetStrings::Fredas),
			std::make_pair(RE::Calendar::Day::kLoredas, UIDayOfWeekSelectorWidgetStrings::Loredas)

		);

		bool UIDayOfWeekSelectorWidget::DrawDayOfWeekSelectorWidget(RE::Calendar::Day& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Source, "ex_dw_sel"),
					day_of_week_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : s_data)
				{
					ImGui::PushID(stl::underlying(e.first));

					const bool selected = (e.first == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							UIL::LS<UIDayOfWeekSelectorWidgetStrings, 3>(e.second, "1"),
							selected))
					{
						a_type = e.first;
						result = true;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			return result;
		}

		const char* UIDayOfWeekSelectorWidget::day_of_week_to_desc(RE::Calendar::Day a_day)
		{
			switch (a_day)
			{
			case RE::Calendar::Day::kSundas:
				return UIL::LS(UIDayOfWeekSelectorWidgetStrings::Sundas);
			case RE::Calendar::Day::kMorndas:
				return UIL::LS(UIDayOfWeekSelectorWidgetStrings::Morndas);
			case RE::Calendar::Day::kTirdas:
				return UIL::LS(UIDayOfWeekSelectorWidgetStrings::Tirdas);
			case RE::Calendar::Day::kMiddas:
				return UIL::LS(UIDayOfWeekSelectorWidgetStrings::Middas);
			case RE::Calendar::Day::kTurdas:
				return UIL::LS(UIDayOfWeekSelectorWidgetStrings::Turdas);
			case RE::Calendar::Day::kFredas:
				return UIL::LS(UIDayOfWeekSelectorWidgetStrings::Fredas);
			case RE::Calendar::Day::kLoredas:
				return UIL::LS(UIDayOfWeekSelectorWidgetStrings::Loredas);
			default:
				return nullptr;
			}
		}
	}
}
#include "pch.h"

#include "UIVariableConditionSourceSelectorWidget.h"

namespace IED
{
	namespace UI
	{

		static constexpr auto s_data = stl::make_array(

			std::make_pair(Data::VariableConditionSource::kAny, UIVariableConditionSourceSelectorWidgetStrings::Any),
			std::make_pair(Data::VariableConditionSource::kSelf, UIVariableConditionSourceSelectorWidgetStrings::Self),
			std::make_pair(Data::VariableConditionSource::kActor, UIVariableConditionSourceSelectorWidgetStrings::Actor),
			std::make_pair(Data::VariableConditionSource::kNPC, UIVariableConditionSourceSelectorWidgetStrings::NPC),
			std::make_pair(Data::VariableConditionSource::kRace, UIVariableConditionSourceSelectorWidgetStrings::Race),
			std::make_pair(Data::VariableConditionSource::kPlayerHorse, UIVariableConditionSourceSelectorWidgetStrings::PlayerHorse)

		);

		bool UIVariableConditionSourceSelectorWidget::DrawVariableConditionSourceSelectorWidget(
			Data::VariableConditionSource& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Source, "ex_vcs_sel"),
					variable_cond_source_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : s_data)
				{
					ImGui::PushID(stl::underlying(e.first));

					bool selected = (e.first == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							UIL::LS<UIVariableConditionSourceSelectorWidgetStrings, 3>(e.second, "1"),
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

		const char* UIVariableConditionSourceSelectorWidget::variable_cond_source_to_desc(
			Data::VariableConditionSource a_type)
		{
			switch (a_type)
			{
			case Data::VariableConditionSource::kAny:
				return UIL::LS(UIVariableConditionSourceSelectorWidgetStrings::Any);
			case Data::VariableConditionSource::kSelf:
				return UIL::LS(UIVariableConditionSourceSelectorWidgetStrings::Self);
			case Data::VariableConditionSource::kActor:
				return UIL::LS(UIVariableConditionSourceSelectorWidgetStrings::Actor);
			case Data::VariableConditionSource::kNPC:
				return UIL::LS(UIVariableConditionSourceSelectorWidgetStrings::NPC);
			case Data::VariableConditionSource::kRace:
				return UIL::LS(UIVariableConditionSourceSelectorWidgetStrings::Race);
			case Data::VariableConditionSource::kPlayerHorse:
				return UIL::LS(UIVariableConditionSourceSelectorWidgetStrings::PlayerHorse);
			default:
				return nullptr;
			}
		}
	}
}
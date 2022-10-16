#include "pch.h"

#include "UIVariableConditionTargetSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIVariableConditionTargetSelectorWidget::data_type UIVariableConditionTargetSelectorWidget::m_data{ {

			{ Data::VariableConditionTarget::kAll, UIVariableConditionTargetSelectorWidgetStrings::All },
			{ Data::VariableConditionTarget::kSelf, UIVariableConditionTargetSelectorWidgetStrings::Self },
			{ Data::VariableConditionTarget::kActor, UIVariableConditionTargetSelectorWidgetStrings::Actor },
			{ Data::VariableConditionTarget::kNPC, UIVariableConditionTargetSelectorWidgetStrings::NPC },
			{ Data::VariableConditionTarget::kRace, UIVariableConditionTargetSelectorWidgetStrings::Race },

		} };

		UIVariableConditionTargetSelectorWidget::UIVariableConditionTargetSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIVariableConditionTargetSelectorWidget::DrawVariableConditionTargetSelectorWidget(
			Data::VariableConditionTarget& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(CommonStrings::Target, "ex_vct_sel"),
					variable_cond_target_to_desc(a_type),
					ImGuiComboFlags_HeightLarge))
			{
				for (auto& e : m_data)
				{
					ImGui::PushID(stl::underlying(e.first));

					bool selected = (e.first == a_type);
					if (selected)
					{
						if (ImGui::IsWindowAppearing())
							ImGui::SetScrollHereY();
					}

					if (ImGui::Selectable(
							LS<UIVariableConditionTargetSelectorWidgetStrings, 3>(e.second, "1"),
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

		const char* UIVariableConditionTargetSelectorWidget::variable_cond_target_to_desc(
			Data::VariableConditionTarget a_type) const
		{
			switch (a_type)
			{
			case Data::VariableConditionTarget::kAll:
				return LS(UIVariableConditionTargetSelectorWidgetStrings::All);
			case Data::VariableConditionTarget::kSelf:
				return LS(UIVariableConditionTargetSelectorWidgetStrings::Self);
			case Data::VariableConditionTarget::kActor:
				return LS(UIVariableConditionTargetSelectorWidgetStrings::Actor);
			case Data::VariableConditionTarget::kNPC:
				return LS(UIVariableConditionTargetSelectorWidgetStrings::NPC);
			case Data::VariableConditionTarget::kRace:
				return LS(UIVariableConditionTargetSelectorWidgetStrings::Race);
			default:
				return nullptr;
			}
		}
	}
}
#include "pch.h"

#include "UIVariableConditionSourceSelectorWidget.h"

namespace IED
{
	namespace UI
	{
		UIVariableConditionSourceSelectorWidget::data_type UIVariableConditionSourceSelectorWidget::m_data{ {

			{ Data::VariableConditionSource::kAny, UIVariableConditionSourceSelectorWidgetStrings::Any },
			{ Data::VariableConditionSource::kSelf, UIVariableConditionSourceSelectorWidgetStrings::Self },
			{ Data::VariableConditionSource::kActor, UIVariableConditionSourceSelectorWidgetStrings::Actor },
			{ Data::VariableConditionSource::kNPC, UIVariableConditionSourceSelectorWidgetStrings::NPC },
			{ Data::VariableConditionSource::kRace, UIVariableConditionSourceSelectorWidgetStrings::Race },
			{ Data::VariableConditionSource::kPlayerHorse, UIVariableConditionSourceSelectorWidgetStrings::PlayerHorse },

		} };

		UIVariableConditionSourceSelectorWidget::UIVariableConditionSourceSelectorWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		bool UIVariableConditionSourceSelectorWidget::DrawVariableConditionSourceSelectorWidget(
			Data::VariableConditionSource& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					LS(CommonStrings::Source, "ex_vcs_sel"),
					variable_cond_source_to_desc(a_type),
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
							LS<UIVariableConditionSourceSelectorWidgetStrings, 3>(e.second, "1"),
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
			Data::VariableConditionSource a_type) const
		{
			switch (a_type)
			{
			case Data::VariableConditionSource::kAny:
				return LS(UIVariableConditionSourceSelectorWidgetStrings::Any);
			case Data::VariableConditionSource::kSelf:
				return LS(UIVariableConditionSourceSelectorWidgetStrings::Self);
			case Data::VariableConditionSource::kActor:
				return LS(UIVariableConditionSourceSelectorWidgetStrings::Actor);
			case Data::VariableConditionSource::kNPC:
				return LS(UIVariableConditionSourceSelectorWidgetStrings::NPC);
			case Data::VariableConditionSource::kRace:
				return LS(UIVariableConditionSourceSelectorWidgetStrings::Race);
			case Data::VariableConditionSource::kPlayerHorse:
				return LS(UIVariableConditionSourceSelectorWidgetStrings::PlayerHorse);
			default:
				return nullptr;
			}
		}
	}
}
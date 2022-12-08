#include "pch.h"

#include "UIVariableSourceSelectorWidget.h"

namespace IED
{
	namespace UI
	{

		static constexpr auto s_data = stl::make_array(

			std::make_pair(Data::VariableSource::kActor, UIVariableSourceSelectorWidgetStrings::Actor),
			std::make_pair(Data::VariableSource::kPlayerHorse, UIVariableSourceSelectorWidgetStrings::PlayerHorse),
			std::make_pair(Data::VariableSource::kMountingActor, UIVariableSourceSelectorWidgetStrings::MountingActor),
			std::make_pair(Data::VariableSource::kMountedActor, UIVariableSourceSelectorWidgetStrings::MountedActor)

		);

		bool UIVariableSourceSelectorWidget::DrawVariableSourceSelectorWidget(
			Data::VariableSource& a_type)
		{
			bool result = false;

			if (ImGui::BeginCombo(
					UIL::LS(CommonStrings::Source, "ex_vs_sel"),
					variable_source_to_desc(a_type),
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
							UIL::LS<UIVariableSourceSelectorWidgetStrings, 3>(e.second, "1"),
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

		const char* UIVariableSourceSelectorWidget::variable_source_to_desc(
			Data::VariableSource a_type)
		{
			switch (a_type)
			{
			case Data::VariableSource::kActor:
				return UIL::LS(UIVariableSourceSelectorWidgetStrings::Actor);
			case Data::VariableSource::kPlayerHorse:
				return UIL::LS(UIVariableSourceSelectorWidgetStrings::PlayerHorse);
			case Data::VariableSource::kMountingActor:
				return UIL::LS(UIVariableSourceSelectorWidgetStrings::MountingActor);
			case Data::VariableSource::kMountedActor:
				return UIL::LS(UIVariableSourceSelectorWidgetStrings::MountedActor);
			default:
				return nullptr;
			}
		}
	}
}
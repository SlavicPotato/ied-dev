#include "pch.h"

#include "UIEditorPanelSettingsGear.h"

#include "IED/Controller/Controller.h"
#include "UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		UIEditorPanelSettingsGear::UIEditorPanelSettingsGear(
			Controller& a_controller) :
			UIEditorPanelSettings(a_controller),
			UILocalizationInterface(a_controller)
		{
		}

		void UIEditorPanelSettingsGear::DrawExtraEditorPanelSettings()
		{
			auto& data = GetEditorPanelSettings();

			ImGui::SameLine(0.0f, 15.0f);

			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			ImGui::SameLine(0.0f, 15.0f);

			if (ImGui::Checkbox(
					LS(UIWidgetCommonStrings::PropagateToEquipmentOverrides, "1"),
					std::addressof(data.eoPropagation)))
			{
				OnEditorPanelSettingsChange();
			}
			DrawTip(UITip::PropagateToEquipmentOverrides);
		}
	}
}
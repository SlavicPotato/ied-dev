#include "pch.h"

#include "UIEditorPanelSettings.h"

namespace IED
{
	namespace UI
	{
		void UIEditorPanelSettings::SetSex(
			Data::ConfigSex a_sex,
			bool            a_sendEvent)
		{
			if (m_sex != a_sex)
			{
				m_sex = a_sex;
				OnSexChanged(a_sex);
			}
		}

		void UIEditorPanelSettings::DrawEditorPanelSettings()
		{
			ImGui::PushID("sex_selector");

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Male, "1"),
					m_sex == Data::ConfigSex::Male))
			{
				SetSex(Data::ConfigSex::Male, true);
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Female, "2"),
					m_sex == Data::ConfigSex::Female))
			{
				SetSex(Data::ConfigSex::Female, true);
			}

			ImGui::SameLine(0.0f, 10.0f);

			auto& data = GetEditorPanelSettings();
			if (ImGui::Checkbox(
					UIL::LS(CommonStrings::Sync, "3"),
					std::addressof(data.sexSync)))
			{
				OnEditorPanelSettingsChange();
			}
			UITipsInterface::DrawTip(UITip::SyncSexes);

			ImGui::PopID();

			ImGui::PushID("edp_extra");
			DrawExtraEditorPanelSettings();
			ImGui::PopID();
		}

		void UIEditorPanelSettings::DrawExtraEditorPanelSettings()
		{
		}

	}
}
#include "pch.h"

#include "UIIntroBanner.h"
#include "UIIntroBannerStrings.h"

#include "UICommon.h"
#include "UIData.h"

#include "IED/Controller/Controller.h"

#include "Drivers/UI.h"

#include "gitparams.h"

namespace IED
{
	namespace UI
	{
		UIIntroBanner::UIIntroBanner(
			Controller& a_controller) :
			UIContextBase(a_controller),
			UILocalizationInterface(a_controller)
		{
		}

		void UIIntroBanner::Draw()
		{
			auto& io = ImGui::GetIO();

			constexpr auto window_flags = ImGuiWindowFlags_NoInputs |
			                              ImGuiWindowFlags_NoMouseInputs |
			                              ImGuiWindowFlags_NoMove |
			                              ImGuiWindowFlags_NoDecoration |
			                              ImGuiWindowFlags_NoBringToFrontOnFocus |
			                              ImGuiWindowFlags_AlwaysAutoResize;

			ImGui::SetNextWindowPos(
				{ io.DisplaySize.x * 0.5f,
			      125.0f },
				ImGuiCond_Always,
				{ 0.5f, 0.5f });

			ImGui::SetNextWindowBgAlpha(0.5f);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_animbg.step());

			if (ImGui::Begin(
					"###ied_intro",
					GetOpenState(),
					window_flags))
			{
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorLightGrey);

				ImGui::Text(
					"%s %s %s",
					PLUGIN_NAME_FULL,
					PLUGIN_VERSION_VERSTRING,
					LS(UIIntroBannerStrings::HeaderAppend));

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				auto& ih = m_controller.GetInputHandlers();

				if (ih.uiOpen.GetKey())
				{
					char buf1[12];

					auto key = UIData::get_control_key_desc(
						UIData::g_controlMap,
						ih.uiOpen.GetKey(),
						buf1);

					ImGui::Text("%s", LS(UIIntroBannerStrings::UIOpenKeys));
					ImGui::SameLine();

					if (ih.uiOpen.GetComboKey())
					{
						char buf2[12];

						auto comboKey = UIData::get_control_key_desc(
							UIData::g_comboControlMap,
							ih.uiOpen.GetComboKey(),
							buf2);

						ImGui::TextColored(
							UICommon::g_colorLightOrange,
							"%s + %s",
							comboKey,
							key);
					}
					else
					{
						ImGui::TextColored(
							UICommon::g_colorLightOrange,
							"%s",
							key);
					}
				}
				else
				{
					ImGui::TextColored(
						UICommon::g_colorWarning,
						"%s",
						LS(UIIntroBannerStrings::NoKeyWarning));
				}

				ImGui::PopStyleColor();
			}

			ImGui::End();

			ImGui::PopStyleVar();
		}
	}
}
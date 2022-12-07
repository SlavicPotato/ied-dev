#include "pch.h"

#include "UIIntroBanner.h"
#include "UIIntroBannerStrings.h"

#include "UIData.h"

#include "IED/Controller/Controller.h"

#include "Drivers/UI.h"
#include "Drivers/UI/Tasks.h"

namespace IED
{
	namespace UI
	{
		UIIntroBanner::UIIntroBanner(
			Tasks::UIRenderTaskBase& a_owner,
			Controller&              a_controller,
			float                    a_voffset) :
			UILocalizationInterface(a_controller),
			m_owner(a_owner),
			m_controller(a_controller),
			m_voffset(a_voffset)
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
			      std::clamp(m_voffset, 0.0f, io.DisplaySize.y * 0.5f) },
				ImGuiCond_Always,
				{ 0.5f, 0.0f });

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

				PluginInterfaceSDS*            intfc;
				PluginInterfaceQueryErrorState intfcErrorState;

				m_controller.GetPluginInterface(intfc, intfcErrorState);

				if (!intfc)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);

					ImGui::Text("%s:", LS(UIIntroBannerStrings::SDSNotFound));
					ImGui::SameLine();

					switch (intfcErrorState)
					{
					case PluginInterfaceQueryErrorState::kDllNotLoaded:
						ImGui::TextUnformatted(LS(UIIntroBannerStrings::SDSNotLoaded));
						break;
					case PluginInterfaceQueryErrorState::kEntryPointNotFound:
						ImGui::TextUnformatted(LS(UIIntroBannerStrings::SDSOutOfDate));
						break;
					default:
						ImGui::TextUnformatted(PluginInterfaceBase::get_error_string(intfcErrorState));
						break;
					}

					ImGui::PopStyleColor();
				}
				else
				{
					const auto pluginVersion = intfc->GetPluginVersion();

					ImGui::Text(
						"%s %u.%u.%u %s",
						intfc->GetPluginName(),
						GET_PLUGIN_VERSION_MAJOR(pluginVersion),
						GET_PLUGIN_VERSION_MINOR(pluginVersion),
						GET_PLUGIN_VERSION_REV(pluginVersion),
						LS(UIIntroBannerStrings::SDSFoundAppend));
				}

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

					if (!m_owner.GetEnabledInMenu())
					{
						ImGui::SameLine();
						ImGui::Text("%s", LS(UIIntroBannerStrings::KeyInfoAppend));
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
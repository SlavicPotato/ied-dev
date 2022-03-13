#include "pch.h"

#include "UICommon.h"

namespace IED
{
	namespace UI
	{
		namespace UICommon
		{
			const ImVec4 g_colorWarning(1.0f, 0.66f, 0.13f, 1.0f);
			const ImVec4 g_colorError(0.84f, 0.26f, 0.2f, 1.0f);
			const ImVec4 g_colorFatalError(1.0f, 0.0f, 0.0f, 1.0f);
			const ImVec4 g_colorOK(0.15f, 0.87f, 0.23f, 1.0f);
			const ImVec4 g_colorGreyed(0.6f, 0.6f, 0.6f, 1.0f);
			const ImVec4 g_colorLightGrey(0.8f, 0.8f, 0.8f, 1.0f);
			const ImVec4 g_colorLightOrange(0.9607f, 0.4941f, 0.2588f, 1.0f);
			const ImVec4 g_colorDarkOrange(0.651f, 0.2039f, 0.0274f, 1.0f);
			const ImVec4 g_colorLimeGreen(0.5333f, 1.0f, 0.3019f, 1.0f);
			const ImVec4 g_colorLightBlue(0.2745f, 0.5803f, 0.9098f, 1.0f);
			const ImVec4 g_colorLightBlue2(0.0941f, 0.7215f, 0.8588f, 1.0f);
			const ImVec4 g_colorPurple(0.6117f, 0.3411f, 0.8509f, 1.0f);

			void HelpMarker(const char* a_desc)
			{
				/*ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
				ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
				ImGui::Button("[?]##tool_tip");
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				ImGui::PopItemFlag();*/
				ImGui::TextDisabled("[?]");
				ToolTip(a_desc, 50.0f);
			}

			void HelpMarkerWarn(const char* a_desc)
			{
				ImGui::TextDisabled("[!]");
				ToolTip(a_desc, 50.0f);
			}

			void ToolTip(const char* a_text, float a_width)
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * a_width);
					ImGui::TextUnformatted(a_text);
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}

			void PushDisabled(bool a_switch)
			{
				if (a_switch)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(
						ImGuiStyleVar_Alpha,
						ImGui::GetStyle().Alpha * 0.5f);
				}
			}

			void PopDisabled(bool a_switch)
			{
				if (a_switch)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}
		}

	}

}
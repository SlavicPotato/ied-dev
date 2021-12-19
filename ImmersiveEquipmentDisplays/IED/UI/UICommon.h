#pragma once

namespace IED
{
	namespace UI
	{
		namespace UICommon
		{
			extern const ImVec4 g_colorWarning;
			extern const ImVec4 g_colorError;
			extern const ImVec4 g_colorFatalError;
			extern const ImVec4 g_colorOK;
			extern const ImVec4 g_colorGreyed;
			// extern const ImVec4 g_colorLightRed;
			extern const ImVec4 g_colorLightOrange;
			extern const ImVec4 g_colorLimeGreen;
			extern const ImVec4 g_colorLightBlue;
			extern const ImVec4 g_colorPurple;

			void HelpMarker(const char* a_desc);
			void ToolTip(const char* a_text, float a_width = 100.0f);

			template <class... Args>
			void ToolTip(float a_width, const char* a_fmt, Args... a_args)
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * a_width);
					ImGui::Text(a_fmt, a_args...);
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}

			template <class... Args>
			void HelpMarkerFormatted(const char* a_desc, Args... a_args)
			{
				ImGui::TextDisabled("[?]");
				ToolTip(100.0f, a_desc, std::forward<Args>(a_args)...);
			}

			void PushDisabled(bool a_switch);
			void PopDisabled(bool a_switch);

		}
	}
}
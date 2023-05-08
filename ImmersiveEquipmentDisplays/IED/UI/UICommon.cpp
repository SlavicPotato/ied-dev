#include "pch.h"

#include "UICommon.h"

#include "IED/ConfigLUIDTag.h"
#include "IED/Controller/IUI.h"

#include <shellapi.h>

#include "Widgets/UIPopupToggleButtonWidget.h"

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
				if (!ImGui::GetCurrentWindow()->SkipItems)
				{
					ImGui::TextDisabled("[?]");
					ToolTip(a_desc, 50.0f);
				}
			}

			void HelpMarkerImportant(const char* a_desc)
			{
				if (!ImGui::GetCurrentWindow()->SkipItems)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorLightOrange);
					ImGui::TextUnformatted("[!]");
					ImGui::PopStyleColor();
					ToolTip(a_desc, 50.0f);
				}
			}

			void ToolTip(const char* a_text, float a_width)
			{
				if (ImGui::GetCurrentWindow()->SkipItems)
				{
					return;
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8, 8 });
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * a_width);
					ImGui::TextUnformatted(a_text);
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
					ImGui::PopStyleVar();
				}
			}

			void ToolTip(float a_width, const char* a_fmt, ...)
			{
				if (ImGui::GetCurrentWindow()->SkipItems)
				{
					return;
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8, 8 });
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * a_width);

					va_list args;
					va_start(args, a_fmt);
					ImGui::TextV(a_fmt, args);
					va_end(args);

					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
					ImGui::PopStyleVar();
				}
			}

			void ToolTip(float a_width, std::function<void()> a_func)
			{
				if (ImGui::GetCurrentWindow()->SkipItems)
				{
					return;
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8, 8 });
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * a_width);
					a_func();
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
					ImGui::PopStyleVar();
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

			void DrawItemUnderline(ImGuiCol a_color)
			{
				auto min = ImGui::GetItemRectMin();
				auto max = ImGui::GetItemRectMax();

				min.y = max.y;

				ImGui::GetWindowDrawList()->AddLine(
					min,
					max,
					ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[a_color]),
					1.0f);
			}

			void DrawURL(const char* a_fmt, const char* a_url, ...)
			{
				if (ImGui::GetCurrentWindow()->SkipItems)
				{
					return;
				}

				va_list args;
				va_start(args, a_url);
				ImGui::TextV(a_fmt, args);
				va_end(args);

				if (ImGui::IsItemHovered())
				{
					DrawItemUnderline(ImGuiCol_Text);

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						ShellExecuteA(
							nullptr,
							"open",
							a_url,
							nullptr,
							nullptr,
							SW_SHOWNORMAL);
					}
				}
			}

			std::string SanitizeInputText(const std::string& a_text)
			{
				using namespace boost::locale::conv;

				return utf_to_utf<char>(a_text, skip);
			}

			std::string SanitizeInputText(const char* a_text)
			{
				using namespace boost::locale::conv;

				return utf_to_utf<char>(a_text, skip);
			}

			void ContextMenu(
				const char*           a_imid,
				std::function<void()> a_func)
			{
				ImGui::PushID(a_imid);

				//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

				UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

				//ImGui::PopStyleVar();

				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::BeginPopup("context_menu"))
				{
					a_func();

					ImGui::EndPopup();
				}

				ImGui::PopID();
			}

			/*bool TextCopyable(const char* a_fmt, ...)
			{
				if (ImGui::GetCurrentWindow()->SkipItems)
				{
					return false;
				}

				{
					va_list args;
					va_start(args, a_fmt);
					ImGui::TextV(a_fmt, args);
					va_end(args);
				}

				if (!ImGui::IsItemHovered())
				{
					return false;
				}

				DrawItemUnderline(ImGuiCol_Text);

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					auto buffer = std::make_unique<char[]>(4096);

					va_list args;
					va_start(args, a_fmt);
					::_vsnprintf_s(buffer.get(), 4096, _TRUNCATE, a_fmt, args);
					va_end(args);

					ImGui::SetClipboardText(buffer.get());

					return true;
				}
				else
				{
					return false;
				}
			}*/

			/*void PushLUID(const Data::configLUIDTagMCG_t& a_id)
			{
				auto& d = a_id.get_tag_data();
				ImGui::PushID(reinterpret_cast<const void*>(d.p1));
				ImGui::PushID(reinterpret_cast<const void*>(d.p2));
			}

			void PopLUID()
			{
				ImGui::PopID();
				ImGui::PopID();
			}*/
		}

	}

}
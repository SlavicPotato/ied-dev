#pragma once

namespace IED
{
	/*namespace Data
	{
		struct configLUIDTagMCG_t;
	}*/

	namespace UI
	{
		namespace UICommon
		{
			extern const ImVec4 g_colorWarning;
			extern const ImVec4 g_colorError;
			extern const ImVec4 g_colorFatalError;
			extern const ImVec4 g_colorOK;
			extern const ImVec4 g_colorGreyed;
			extern const ImVec4 g_colorLightGrey;
			// extern const ImVec4 g_colorLightRed;
			extern const ImVec4 g_colorLightOrange;
			extern const ImVec4 g_colorDarkOrange;
			extern const ImVec4 g_colorLimeGreen;
			extern const ImVec4 g_colorLightBlue;
			extern const ImVec4 g_colorLightBlue2;
			extern const ImVec4 g_colorPurple;

			void HelpMarker(const char* a_desc);
			void HelpMarkerImportant(const char* a_desc);

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

			template <class Tf>
			void ToolTip(float a_width, Tf a_func)
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * a_width);
					a_func();
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

			/*void PushLUID(const Data::configLUIDTagMCG_t& a_id);
			void PopLUID();*/

			template <
				class T,
				T _Min,
				T _Max,
				T _Speed>
			requires std::is_floating_point_v<T>
			struct float_anim_t
			{
			public:
				constexpr T step()
				{
					auto result = current;
					current     = std::clamp(current + static_cast<T>(ImGui::GetIO().DeltaTime) * _Speed, _Min, _Max);
					return result;
				}

			private:
				T current{ _Min };
			};

		}
	}
}
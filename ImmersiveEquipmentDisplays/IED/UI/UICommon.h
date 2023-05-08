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
			void ToolTip(float a_width, const char* a_fmt, ...);
			void ToolTip(float a_width, std::function<void()> a_func);

			void PushDisabled(bool a_switch);
			void PopDisabled(bool a_switch);

			/*void PushLUID(const Data::configLUIDTagMCG_t& a_id);
			void PopLUID();*/

			void DrawItemUnderline(ImGuiCol a_color);

			void DrawURL(const char* a_fmt, const char* a_url, ...);

			/*template <class... Args>
			bool TextCopyable(const char* a_fmt, Args... a_args)
			{
				if (ImGui::GetCurrentWindow()->SkipItems)
				{
					return false;
				}

				ImGui::Text(a_fmt, a_args...);

				if (!ImGui::IsItemHovered())
				{
					return false;
				}

				UICommon::DrawItemUnderline(ImGuiCol_Text);

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					auto buffer = std::make_unique<char[]>(4096);

					::_snprintf_s(buffer.get(), 4096, _TRUNCATE, a_fmt, a_args...);

					ImGui::SetClipboardText(buffer.get());

					return true;
				}
				else
				{
					return false;
				}
			}*/

			namespace concepts
			{
				template <class T>
				concept accept_float_anim_t = std::is_floating_point_v<T>;
			}

			template <
				concepts::accept_float_anim_t T,
				T                             _Min,
				T                             _Max,
				T                             _Speed>
			struct float_anim_stepper
			{
			public:
				constexpr T step()
				{
					const auto result = current;

					current = std::clamp(
						current + static_cast<T>(ImGui::GetIO().DeltaTime) * _Speed,
						_Min,
						_Max);

					return result;
				}

				constexpr T lerp()
				{
					const auto result = current;

					current = std::clamp(
						std::lerp(
							current,
							_Max,
							static_cast<T>(ImGui::GetIO().DeltaTime) * _Speed),
						std::min(current, _Min),
						std::max(current, _Max));

					return result;
				}

				constexpr void reset() noexcept
				{
					current = _Min;
				}

			private:
				T current{ _Min };
			};

			std::string SanitizeInputText(const std::string& a_text);
			std::string SanitizeInputText(const char* a_text);

			void ContextMenu(
				const char*            a_imid,
				std::function<void()>  a_func);

		}
	}
}
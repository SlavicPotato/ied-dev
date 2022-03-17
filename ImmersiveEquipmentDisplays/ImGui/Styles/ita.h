#pragma once

// https://gist.github.com/dougbinks/8089b4bbaccaaf6fa204236978d165a9

namespace IED
{
	namespace UI
	{
		namespace Styles
		{
			namespace ITA
			{
				enum class Template
				{
					Dark,
					Light,
					Classic
				};

				inline void Setup(ImGuiStyle& a_style, Template a_templ, float a_alpha)
				{
					switch (a_templ)
					{
					case Template::Light:
						ImGui::StyleColorsLight(std::addressof(a_style));
						break;
					case Template::Classic:
						ImGui::StyleColorsClassic(std::addressof(a_style));
						break;
					default:
						ImGui::StyleColorsDark(std::addressof(a_style));
						break;
					}

					for (ImGuiCol i = 0; i <= ImGuiCol_COUNT; i++)
					{
						auto  ei  = static_cast<ImGuiCol_>(i);
						auto& col = a_style.Colors[i];

						if ((ImGuiCol_ModalWindowDimBg != ei) &&
						    (ImGuiCol_NavWindowingDimBg != ei) &&
						    (col.w < 1.00f || (ImGuiCol_FrameBg == ei) || (ImGuiCol_WindowBg == ei) || (ImGuiCol_ChildBg == ei)))
						{
							col.w *= a_alpha;
						}
					}

					a_style.ChildBorderSize  = 1.0f;
					a_style.FrameBorderSize  = 0.0f;
					a_style.PopupBorderSize  = 1.0f;
					a_style.WindowBorderSize = 0.0f;
					a_style.Alpha            = 1.0f;
					a_style.WindowRounding   = 3.0f;
					a_style.FrameRounding    = 2.0f;
					a_style.PopupRounding    = 2.0f;
				}
			}
		}
	}
}
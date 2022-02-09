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
				inline void Setup(ImGuiStyle& a_style, bool a_dark, float a_alpha)
				{
					if (a_dark)
					{
						ImGui::StyleColorsDark(std::addressof(a_style));
					}
					else
					{
						ImGui::StyleColorsLight(std::addressof(a_style));
					}

					for (ImGuiCol i = 0; i <= ImGuiCol_COUNT; i++)
					{
						auto ei = static_cast<ImGuiCol_>(i);
						auto& col = a_style.Colors[i];

						if ((ImGuiCol_ModalWindowDimBg != ei) &&
						    (ImGuiCol_NavWindowingDimBg != ei) &&
						    (col.w < 1.00f || (ImGuiCol_FrameBg == ei) || (ImGuiCol_WindowBg == ei) || (ImGuiCol_ChildBg == ei)))
						{
							col.w = a_alpha * col.w;
						}
					}

					a_style.ChildBorderSize = 1.0f;
					a_style.FrameBorderSize = 0.0f;
					a_style.PopupBorderSize = 1.0f;
					a_style.WindowBorderSize = 0.0f;
					a_style.FrameRounding = 3.0f;
					a_style.Alpha = 1.0f;
				}
			}
		}
	}
}
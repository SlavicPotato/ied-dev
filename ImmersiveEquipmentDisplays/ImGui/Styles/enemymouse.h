#pragma once

// https://gist.github.com/enemymouse/c8aa24e247a1d7b9fc33d45091cbb8f0

namespace IED
{
	namespace UI
	{
		namespace Styles
		{
			namespace EnemyMouse
			{
				inline void Setup(ImGuiStyle& a_style)
				{
					ImGui::StyleColorsDark(std::addressof(a_style));

					a_style.Alpha          = 1.0;
					a_style.WindowRounding = 3;
					a_style.GrabRounding   = 1;
					a_style.GrabMinSize    = 20;
					a_style.FrameRounding  = 3;

					a_style.Colors[ImGuiCol_Text]                 = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
					a_style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
					a_style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
					a_style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
					a_style.Colors[ImGuiCol_Border]               = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
					a_style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
					a_style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
					a_style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
					a_style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
					a_style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
					a_style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
					a_style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
					a_style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
					a_style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
					a_style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
					a_style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
					a_style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
					a_style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
					a_style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
					a_style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
					a_style.Colors[ImGuiCol_Button]               = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
					a_style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
					a_style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
					a_style.Colors[ImGuiCol_Header]               = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
					a_style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
					a_style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
					a_style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
					a_style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
					a_style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
					a_style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
					a_style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
					a_style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
					a_style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
					a_style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
				}
			}
		}
	}
}
#pragma once

// https://github.com/GraphicsProgramming/dear-imgui-styles

#include "adobe-spectrum/imgui_spectrum.h"

namespace IED
{
	namespace UI
	{
		namespace Styles
		{
			namespace SpectrumDark
			{
				inline void Setup(ImGuiStyle& a_style)
				{
					using namespace ImGui;

					a_style.ChildBorderSize  = 1.0f;
					a_style.FrameBorderSize  = 0.0f;
					a_style.PopupBorderSize  = 1.0f;
					a_style.WindowBorderSize = 0.0f;
					a_style.Alpha            = 1.0f;

					a_style.WindowRounding   = 6.0f;
					a_style.FrameRounding    = 3.0f;
					a_style.PopupRounding    = 2.0f;
					a_style.GrabRounding     = 4.0f;

					auto& colors = a_style.Colors;

					colors[ImGuiCol_Text]                  = ColorConvertU32ToFloat4(Spectrum::GRAY800);  // text on hovered controls is gray900
					colors[ImGuiCol_TextDisabled]          = ColorConvertU32ToFloat4(Spectrum::GRAY500);
					colors[ImGuiCol_WindowBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
					colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
					colors[ImGuiCol_PopupBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
					colors[ImGuiCol_Border]                = ColorConvertU32ToFloat4(Spectrum::GRAY300);
					colors[ImGuiCol_BorderShadow]          = ColorConvertU32ToFloat4(Spectrum::Static::NONE);  // We don't want shadows. Ever.
					colors[ImGuiCol_FrameBg]               = ColorConvertU32ToFloat4(Spectrum::GRAY75);        // this isnt right, spectrum does not do this, but it's a good fallback
					colors[ImGuiCol_FrameBgHovered]        = ColorConvertU32ToFloat4(Spectrum::GRAY50);
					colors[ImGuiCol_FrameBgActive]         = ColorConvertU32ToFloat4(Spectrum::GRAY200);
					colors[ImGuiCol_TitleBg]               = ColorConvertU32ToFloat4(Spectrum::GRAY300);  // those titlebar values are totally made up, spectrum does not have this.
					colors[ImGuiCol_TitleBgActive]         = ColorConvertU32ToFloat4(Spectrum::GRAY200);
					colors[ImGuiCol_TitleBgCollapsed]      = ColorConvertU32ToFloat4(Spectrum::GRAY400);
					colors[ImGuiCol_MenuBarBg]             = ColorConvertU32ToFloat4(Spectrum::GRAY100);
					colors[ImGuiCol_ScrollbarBg]           = ColorConvertU32ToFloat4(Spectrum::GRAY100);  // same as regular background
					colors[ImGuiCol_ScrollbarGrab]         = ColorConvertU32ToFloat4(Spectrum::GRAY400);
					colors[ImGuiCol_ScrollbarGrabHovered]  = ColorConvertU32ToFloat4(Spectrum::GRAY600);
					colors[ImGuiCol_ScrollbarGrabActive]   = ColorConvertU32ToFloat4(Spectrum::GRAY700);
					colors[ImGuiCol_CheckMark]             = ColorConvertU32ToFloat4(Spectrum::BLUE500);
					colors[ImGuiCol_SliderGrab]            = ColorConvertU32ToFloat4(Spectrum::GRAY700);
					colors[ImGuiCol_SliderGrabActive]      = ColorConvertU32ToFloat4(Spectrum::GRAY800);
					colors[ImGuiCol_Button]                = ColorConvertU32ToFloat4(Spectrum::GRAY75);  // match default button to Spectrum's 'Action Button'.
					colors[ImGuiCol_ButtonHovered]         = ColorConvertU32ToFloat4(Spectrum::GRAY50);
					colors[ImGuiCol_ButtonActive]          = ColorConvertU32ToFloat4(Spectrum::GRAY200);
					colors[ImGuiCol_Header]                = ColorConvertU32ToFloat4(Spectrum::BLUE400);
					colors[ImGuiCol_HeaderHovered]         = ColorConvertU32ToFloat4(Spectrum::BLUE500);
					colors[ImGuiCol_HeaderActive]          = ColorConvertU32ToFloat4(Spectrum::BLUE600);
					colors[ImGuiCol_Separator]             = ColorConvertU32ToFloat4(Spectrum::GRAY400);
					colors[ImGuiCol_SeparatorHovered]      = ColorConvertU32ToFloat4(Spectrum::GRAY600);
					colors[ImGuiCol_SeparatorActive]       = ColorConvertU32ToFloat4(Spectrum::GRAY700);
					colors[ImGuiCol_ResizeGrip]            = ColorConvertU32ToFloat4(Spectrum::GRAY400);
					colors[ImGuiCol_ResizeGripHovered]     = ColorConvertU32ToFloat4(Spectrum::GRAY600);
					colors[ImGuiCol_ResizeGripActive]      = ColorConvertU32ToFloat4(Spectrum::GRAY700);
					colors[ImGuiCol_PlotLines]             = ColorConvertU32ToFloat4(Spectrum::BLUE400);
					colors[ImGuiCol_PlotLinesHovered]      = ColorConvertU32ToFloat4(Spectrum::BLUE600);
					colors[ImGuiCol_PlotHistogram]         = ColorConvertU32ToFloat4(Spectrum::BLUE400);
					colors[ImGuiCol_PlotHistogramHovered]  = ColorConvertU32ToFloat4(Spectrum::BLUE600);
					colors[ImGuiCol_TextSelectedBg]        = ColorConvertU32ToFloat4((Spectrum::BLUE400 & 0x00FFFFFF) | 0x33000000);
					colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
					colors[ImGuiCol_NavHighlight]          = ColorConvertU32ToFloat4((Spectrum::GRAY900 & 0x00FFFFFF) | 0x0A000000);
					colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
					colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
					colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
				}
			}
		}
	}
}

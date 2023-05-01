/*
	Uses style editing code from ImGui demos
*/

#include "pch.h"

#include "UIStyleEditorWidget.h"

#include "UIStyleEditorWidgetStrings.h"

#include "IED/UI/UITips.h"

namespace IED
{
	namespace UI
	{
		UIStyleEditorWidget::UIStyleEditorWidget() :
			m_colorFilter(true)
		{
		}

		bool UIStyleEditorWidget::DrawStyleEditor(Data::ImGuiStyleHolder& a_data)
		{
			bool result = false;

			ImGui::PushID("style_editor");

			result |= DrawTabBar(a_data.style);

			ImGui::PopID();

			return result;
		}

		bool UIStyleEditorWidget::DrawTabBar(ImGuiStyle& a_data)
		{
			bool result = false;

			if (ImGui::BeginTabBar(
					"##tabs",
					ImGuiTabBarFlags_NoTooltip |
						ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
			{
				result |= DrawSizesTab(a_data);
				result |= DrawColorsTab(a_data);
				result |= DrawRenderingTab(a_data);

				ImGui::EndTabBar();
			}

			return result;
		}

		bool UIStyleEditorWidget::DrawSizesTab(ImGuiStyle& a_data)
		{
			bool result = false;

			if (ImGui::BeginTabItem(UIL::LS(UIStyleEditorWidgetStrings::TabSizes, "tab_sizes")))
			{
				if (ImGui::BeginChild("size_child"))
				{
					ImGui::SeparatorText(UIL::LS(UIStyleEditorWidgetStrings::SizesSectMain));
					result |= ImGui::SliderFloat2("WindowPadding", (float*)&a_data.WindowPadding, 0.0f, 20.0f, "%.0f");
					result |= ImGui::SliderFloat2("FramePadding", (float*)&a_data.FramePadding, 0.0f, 20.0f, "%.0f");
					result |= ImGui::SliderFloat2("CellPadding", (float*)&a_data.CellPadding, 0.0f, 20.0f, "%.0f");
					result |= ImGui::SliderFloat2("ItemSpacing", (float*)&a_data.ItemSpacing, 0.0f, 20.0f, "%.0f");
					result |= ImGui::SliderFloat2("ItemInnerSpacing", (float*)&a_data.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
					result |= ImGui::SliderFloat2("TouchExtraPadding", (float*)&a_data.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
					result |= ImGui::SliderFloat("IndentSpacing", &a_data.IndentSpacing, 0.0f, 30.0f, "%.0f");
					result |= ImGui::SliderFloat("ScrollbarSize", &a_data.ScrollbarSize, 1.0f, 20.0f, "%.0f");
					result |= ImGui::SliderFloat("GrabMinSize", &a_data.GrabMinSize, 1.0f, 20.0f, "%.0f");

					ImGui::SeparatorText(UIL::LS(UIStyleEditorWidgetStrings::SizesSectBorders));
					result |= ImGui::SliderFloat("WindowBorderSize", &a_data.WindowBorderSize, 0.0f, 6.0f, "%.0f");
					result |= ImGui::SliderFloat("ChildBorderSize", &a_data.ChildBorderSize, 0.0f, 6.0f, "%.0f");
					result |= ImGui::SliderFloat("PopupBorderSize", &a_data.PopupBorderSize, 0.0f, 6.0f, "%.0f");
					result |= ImGui::SliderFloat("FrameBorderSize", &a_data.FrameBorderSize, 0.0f, 4.0f, "%.0f");
					result |= ImGui::SliderFloat("TabBorderSize", &a_data.TabBorderSize, 0.0f, 4.0f, "%.0f");

					ImGui::SeparatorText(UIL::LS(UIStyleEditorWidgetStrings::SizesSectRounding));
					result |= ImGui::SliderFloat("WindowRounding", &a_data.WindowRounding, 0.0f, 12.0f, "%.0f");
					result |= ImGui::SliderFloat("ChildRounding", &a_data.ChildRounding, 0.0f, 12.0f, "%.0f");
					result |= ImGui::SliderFloat("FrameRounding", &a_data.FrameRounding, 0.0f, 12.0f, "%.0f");
					result |= ImGui::SliderFloat("PopupRounding", &a_data.PopupRounding, 0.0f, 12.0f, "%.0f");
					result |= ImGui::SliderFloat("ScrollbarRounding", &a_data.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
					result |= ImGui::SliderFloat("GrabRounding", &a_data.GrabRounding, 0.0f, 12.0f, "%.0f");
					result |= ImGui::SliderFloat("TabRounding", &a_data.TabRounding, 0.0f, 12.0f, "%.0f");

					ImGui::SeparatorText(UIL::LS(UIStyleEditorWidgetStrings::SizesSectWidget));
					result |= ImGui::SliderFloat2("WindowTitleAlign", (float*)&a_data.WindowTitleAlign, 0.0f, 1.0f, "%.2f");

					int window_menu_button_position = a_data.WindowMenuButtonPosition + 1;
					if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
					{
						a_data.WindowMenuButtonPosition = window_menu_button_position - 1;
						result                          = true;
					}

					ImGui::Combo("ColorButtonPosition", (int*)&a_data.ColorButtonPosition, "Left\0Right\0");
					result |= ImGui::SliderFloat2("ButtonTextAlign", (float*)&a_data.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
					UITipsInterface::DrawTip(UITip::ButtonTextAlign);
					result |= ImGui::SliderFloat2("SelectableTextAlign", (float*)&a_data.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
					UITipsInterface::DrawTip(UITip::SelectableTextAlign);
					result |= ImGui::SliderFloat("SeparatorTextBorderSize", &a_data.SeparatorTextBorderSize, 0.0f, 10.0f, "%.0f");
					result |= ImGui::SliderFloat2("SeparatorTextAlign", (float*)&a_data.SeparatorTextAlign, 0.0f, 1.0f, "%.2f");
					result |= ImGui::SliderFloat2("SeparatorTextPadding", (float*)&a_data.SeparatorTextPadding, 0.0f, 40.0f, "%0.f");
					result |= ImGui::SliderFloat("LogSliderDeadzone", &a_data.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");

					ImGui::SeparatorText(UIL::LS(UIStyleEditorWidgetStrings::SizesSectMisc));
					result |= ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&a_data.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
					UITipsInterface::DrawTip(UITip::DisplaySafeAreaPadding);
				}

				ImGui::EndChild();
				ImGui::EndTabItem();
			}

			return result;
		}

		bool UIStyleEditorWidget::DrawColorsTab(ImGuiStyle& a_data)
		{
			bool result = false;

			if (ImGui::BeginTabItem(UIL::LS(UIStyleEditorWidgetStrings::TabColors, "tab_col")))
			{
				DrawColorFilterTree();
				ImGui::Separator();

				if (ImGui::BeginChild(
						"col_child",
						{ 0, 0 },
						false,
						ImGuiWindowFlags_NavFlattened))
				{
					for (ImGuiCol i = 0; i < ImGuiCol_COUNT; i++)
					{
						const auto name = ImGui::GetStyleColorName(i);
						if (!m_colorFilter.Test(name))
						{
							continue;
						}

						ImGui::PushID(i);
						result |= ImGui::ColorEdit4("##color", (float*)&a_data.Colors[i], ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
						ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
						ImGui::TextUnformatted(name);
						ImGui::PopID();
					}
				}

				ImGui::EndChild();
				ImGui::EndTabItem();
			}

			return result;
		}

		bool UIStyleEditorWidget::DrawRenderingTab(ImGuiStyle& a_data)
		{
			bool result = false;

			if (ImGui::BeginTabItem(UIL::LS(UIStyleEditorWidgetStrings::TabRendering, "tab_rend")))
			{
				if (ImGui::BeginChild("rend_child"))
				{
					result |= ImGui::Checkbox("Anti-aliased lines", &a_data.AntiAliasedLines);
					UITipsInterface::DrawTip(UITip::AntiAliasedLines);

					result |= ImGui::Checkbox("Anti-aliased lines use texture", &a_data.AntiAliasedLinesUseTex);
					UITipsInterface::DrawTip(UITip::AntiAliasedLinesUseTex);

					result |= ImGui::Checkbox("Anti-aliased fill", &a_data.AntiAliasedFill);
					ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
					result |= ImGui::DragFloat("Curve Tessellation Tolerance", &a_data.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
					if (a_data.CurveTessellationTol < 0.10f)
						a_data.CurveTessellationTol = 0.10f;

					// When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
					result |= ImGui::DragFloat("Circle Tessellation Max Error", &a_data.CircleTessellationMaxError, 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
					const bool show_samples = ImGui::IsItemActive();
					if (show_samples)
						ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());

					if (show_samples && ImGui::BeginTooltip())
					{
						ImGui::TextUnformatted("(R = radius, N = number of segments)");
						ImGui::Spacing();
						ImDrawList* draw_list        = ImGui::GetWindowDrawList();
						const float min_widget_width = ImGui::CalcTextSize("N: MMM\nR: MMM").x;
						for (int n = 0; n < 8; n++)
						{
							constexpr float RAD_MIN = 5.0f;
							constexpr float RAD_MAX = 70.0f;
							const float     rad     = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

							ImGui::BeginGroup();

							ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

							const float canvas_width = std::max(min_widget_width, rad * 2.0f);
							const float offset_x     = std::floorf(canvas_width * 0.5f);
							const float offset_y     = std::floorf(RAD_MAX);

							const ImVec2 p1 = ImGui::GetCursorScreenPos();
							draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
							ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

							ImGui::EndGroup();
							ImGui::SameLine();
						}
						ImGui::EndTooltip();
					}
					UITipsInterface::DrawTip(UITip::CircleTessellationMaxError);

					result |= ImGui::DragFloat("Global Alpha", &a_data.Alpha, 0.005f, 0.20f, 1.0f, "%.2f");  // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
					result |= ImGui::DragFloat("Disabled Alpha", &a_data.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f");
					UITipsInterface::DrawTip(UITip::DisabledAlpha);
					ImGui::PopItemWidth();
				}

				ImGui::EndChild();
				ImGui::EndTabItem();
			}

			return result;
		}

		void UIStyleEditorWidget::DrawColorFilterTree()
		{
			if (ImGui::TreeNodeEx(
					UIL::LS(CommonStrings::Filter, "col_filter"),
					ImGuiTreeNodeFlags_SpanAvailWidth))
			{
				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				ImGui::Spacing();

				m_colorFilter.Draw();

				ImGui::Spacing();

				ImGui::PopItemWidth();

				ImGui::TreePop();
			}
		}
	}
}
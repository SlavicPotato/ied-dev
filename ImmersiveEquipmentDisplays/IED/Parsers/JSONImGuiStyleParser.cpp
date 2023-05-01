#include "pch.h"

#include "JSONImGuiStyleParser.h"

#include "JSONImVec2Parser.h"
#include "JSONImVec4Parser.h"

namespace IED
{
	static constexpr std::uint32_t CURRENT_VERSION = 1;

	namespace Serialization
	{
		template <>
		bool Parser<ImGuiStyle>::Parse(
			const Json::Value& a_in,
			ImGuiStyle&        a_out) const
		{
			JSON_PARSE_VERSION()

			const Parser<ImVec2> v2parser(m_state);
			const Parser<ImVec4> v4parser(m_state);

			auto& data = a_in["data"];

			a_out.Alpha         = data["alpha"].asFloat();
			a_out.DisabledAlpha = data["disabled_alpha"].asFloat();

			v2parser.Parse(data["window_padding"], a_out.WindowPadding);

			a_out.WindowRounding   = data["window_rounding"].asFloat();
			a_out.WindowBorderSize = data["window_rounding_size"].asFloat();

			v2parser.Parse(data["window_min_size"], a_out.WindowMinSize);
			v2parser.Parse(data["window_title_align"], a_out.WindowTitleAlign);

			a_out.WindowMenuButtonPosition = data["window_menu_button_position"].asInt();
			a_out.ChildRounding            = data["child_rounding"].asFloat();
			a_out.ChildBorderSize          = data["child_border_size"].asFloat();
			a_out.PopupRounding            = data["popup_rounding"].asFloat();
			a_out.PopupBorderSize          = data["popup_border_size"].asFloat();

			v2parser.Parse(data["frame_padding"], a_out.FramePadding);

			a_out.FrameRounding   = data["frame_rounding"].asFloat();
			a_out.FrameBorderSize = data["frame_border_size"].asFloat();

			v2parser.Parse(data["item_spacing"], a_out.ItemSpacing);
			v2parser.Parse(data["item_inner_spacing"], a_out.ItemInnerSpacing);
			v2parser.Parse(data["cell_padding"], a_out.CellPadding);
			v2parser.Parse(data["touch_extra_padding"], a_out.TouchExtraPadding);

			a_out.IndentSpacing             = data["indent_spacing"].asFloat();
			a_out.ColumnsMinSpacing         = data["columns_min_spacing"].asFloat();
			a_out.ScrollbarSize             = data["scollbar_size"].asFloat();
			a_out.ScrollbarRounding         = data["scollbar_rounding"].asFloat();
			a_out.GrabMinSize               = data["grab_min_size"].asFloat();
			a_out.GrabRounding              = data["grab_rounding"].asFloat();
			a_out.LogSliderDeadzone         = data["log_slider_deadzone"].asFloat();
			a_out.TabRounding               = data["tab_rounding"].asFloat();
			a_out.TabBorderSize             = data["tab_border_size"].asFloat();
			a_out.TabMinWidthForCloseButton = data["tab_min_width_for_close_button"].asFloat();
			a_out.ColorButtonPosition       = data["color_button_position"].asInt();

			v2parser.Parse(data["button_text_align"], a_out.ButtonTextAlign);
			v2parser.Parse(data["selectable_text_align"], a_out.SelectableTextAlign);

			a_out.SeparatorTextBorderSize = data["separator_text_border_size"].asFloat();

			v2parser.Parse(data["separator_text_align"], a_out.SeparatorTextAlign);
			v2parser.Parse(data["separator_text_padding"], a_out.SeparatorTextPadding);
			v2parser.Parse(data["display_window_padding"], a_out.DisplayWindowPadding);
			v2parser.Parse(data["display_safe_area_padding"], a_out.DisplaySafeAreaPadding);

			a_out.MouseCursorScale           = data["mouse_cursor_scale"].asFloat();
			a_out.AntiAliasedLines           = data["anti_aliased_lines"].asBool();
			a_out.AntiAliasedLinesUseTex     = data["anti_aliased_lines_use_tex"].asBool();
			a_out.AntiAliasedFill            = data["anti_aliased_fill"].asBool();
			a_out.CurveTessellationTol       = data["curve_tessellation_tol"].asFloat();
			a_out.CircleTessellationMaxError = data["circle_tesselation_max_error"].asFloat();

			auto& cols = data["colors"];

			if (cols.size() != std::size(a_out.Colors))
			{
				throw parser_exception(__FUNCTION__ ": bad color array size");
			}

			for (auto it = cols.begin(); it != cols.end(); ++it)
			{
				const auto index = it.index();

				if (index >= std::size(a_out.Colors))
				{
					throw parser_exception(__FUNCTION__ ": bad color index");
				}

				v4parser.Parse(*it, a_out.Colors[index]);
			}

			return true;
		}

		template <>
		void Parser<ImGuiStyle>::Create(
			const ImGuiStyle& a_data,
			Json::Value&      a_out) const
		{
			const Parser<ImVec2> v2parser(m_state);
			const Parser<ImVec4> v4parser(m_state);

			auto& data = (a_out["data"] = Json::Value(Json::objectValue));

			data["alpha"]          = a_data.Alpha;
			data["disabled_alpha"] = a_data.DisabledAlpha;

			v2parser.Create(a_data.WindowPadding, data["window_padding"]);

			data["window_rounding"]      = a_data.WindowRounding;
			data["window_rounding_size"] = a_data.WindowBorderSize;

			v2parser.Create(a_data.WindowMinSize, data["window_min_size"]);
			v2parser.Create(a_data.WindowTitleAlign, data["window_title_align"]);

			data["window_menu_button_position"] = a_data.WindowMenuButtonPosition;
			data["child_rounding"]              = a_data.ChildRounding;
			data["child_border_size"]           = a_data.ChildBorderSize;
			data["popup_rounding"]              = a_data.PopupRounding;
			data["popup_border_size"]           = a_data.PopupBorderSize;

			v2parser.Create(a_data.FramePadding, data["frame_padding"]);

			data["frame_rounding"]    = a_data.FrameRounding;
			data["frame_border_size"] = a_data.FrameBorderSize;

			v2parser.Create(a_data.ItemSpacing, data["item_spacing"]);
			v2parser.Create(a_data.ItemInnerSpacing, data["item_inner_spacing"]);
			v2parser.Create(a_data.CellPadding, data["cell_padding"]);
			v2parser.Create(a_data.TouchExtraPadding, data["touch_extra_padding"]);

			data["indent_spacing"]                 = a_data.IndentSpacing;
			data["columns_min_spacing"]            = a_data.ColumnsMinSpacing;
			data["scollbar_size"]                  = a_data.ScrollbarSize;
			data["scollbar_rounding"]              = a_data.ScrollbarRounding;
			data["grab_min_size"]                  = a_data.GrabMinSize;
			data["grab_rounding"]                  = a_data.GrabRounding;
			data["log_slider_deadzone"]            = a_data.LogSliderDeadzone;
			data["tab_rounding"]                   = a_data.TabRounding;
			data["tab_border_size"]                = a_data.TabBorderSize;
			data["tab_min_width_for_close_button"] = a_data.TabMinWidthForCloseButton;
			data["color_button_position"]          = a_data.ColorButtonPosition;

			v2parser.Create(a_data.ButtonTextAlign, data["button_text_align"]);
			v2parser.Create(a_data.SelectableTextAlign, data["selectable_text_align"]);

			data["separator_text_border_size"] = a_data.SeparatorTextBorderSize;

			v2parser.Create(a_data.SeparatorTextAlign, data["separator_text_align"]);
			v2parser.Create(a_data.SeparatorTextPadding, data["separator_text_padding"]);
			v2parser.Create(a_data.DisplayWindowPadding, data["display_window_padding"]);
			v2parser.Create(a_data.DisplaySafeAreaPadding, data["display_safe_area_padding"]);

			data["mouse_cursor_scale"]           = a_data.MouseCursorScale;
			data["anti_aliased_lines"]           = a_data.AntiAliasedLines;
			data["anti_aliased_lines_use_tex"]   = a_data.AntiAliasedLinesUseTex;
			data["anti_aliased_fill"]            = a_data.AntiAliasedFill;
			data["curve_tessellation_tol"]       = a_data.CurveTessellationTol;
			data["circle_tesselation_max_error"] = a_data.CircleTessellationMaxError;

			auto& cols = (data["colors"] = Json::Value(Json::arrayValue));

			for (auto& e : a_data.Colors)
			{
				v4parser.Create(e, cols.append(Json::arrayValue));
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
#include "pch.h"

#include "JSONConfigKeyPairParser.h"
#include "JSONSettingsCollapsibleStatesParser.h"
#include "JSONSettingsEditorPanelParser.h"
#include "JSONSettingsImportExport.h"
#include "JSONSettingsProfileEditorParser.h"
#include "JSONSettingsUserInterfaceParser.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::UserInterface>::Parse(
			const Json::Value&                  a_in,
			Data::SettingHolder::UserInterface& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::SettingHolder::EditorPanel>   editorPanelParser(m_state);
			Parser<Data::SettingHolder::ProfileEditor> profileEditorParser(m_state);
			Parser<Data::ConfigKeyPair>                controlsParser(m_state);
			Parser<Data::SettingHolder::ImportExport>  ieParser(m_state);
			Parser<UI::UIData::UICollapsibleStates>    colStatesParser(m_state);

			if (!editorPanelParser.Parse(data["slot_editor"], a_out.slotEditor))
			{
				return false;
			}

			if (!editorPanelParser.Parse(data["custom_editor"], a_out.customEditor))
			{
				return false;
			}

			if (!editorPanelParser.Parse(data["transform_editor"], a_out.transformEditor))
			{
				return false;
			}

			if (!profileEditorParser.Parse(
					data["slot_profile_editor"],
					a_out.slotProfileEditor))
			{
				return false;
			}

			if (!profileEditorParser.Parse(
					data["custom_profile_editor"],
					a_out.customProfileEditor))
			{
				return false;
			}

			if (!profileEditorParser.Parse(
					data["transform_profile_editor"],
					a_out.transformProfileEditor))
			{
				return false;
			}

			if (!ieParser.Parse(
					data["import_export"],
					a_out.importExport))
			{
				return false;
			}

			if (!colStatesParser.Parse(
					data["settings_col_states"],
					a_out.settingsColStates))
			{
				return false;
			}

			if (!colStatesParser.Parse(
					data["stats_col_states"],
					a_out.statsColStates))
			{
				return false;
			}

			auto lastPanel = data.get("last_editor_panel", stl::underlying(UI::UIEditorPanel::Slot)).asUInt();

			switch (lastPanel)
			{
			case 1:
				a_out.lastPanel = UI::UIEditorPanel::Custom;
				break;
			default:
				a_out.lastPanel = UI::UIEditorPanel::Slot;
				break;
			}

			if (auto& keys = data["toggle_keys"])
			{
				if (!controlsParser.Parse(
						keys,
						*a_out.openKeys))
				{
					return false;
				}

				a_out.openKeys.mark(true);
			}

			a_out.enableControlLock    = data.get("enable_control_lock", true).asBool();
			a_out.enableFreezeTime     = data.get("enable_freeze_time", false).asBool();
			a_out.enableRestrictions   = data.get("enable_restrictions", false).asBool();
			a_out.selectCrosshairActor = data.get("select_crosshair_actor", true).asBool();
			a_out.scale                = data.get("scale", 1.0f).asFloat();
			a_out.logLimit             = data.get("log_limit", 200).asUInt();
			a_out.logShowTimestamps    = data.get("log_show_ts", true).asBool();

			auto& levels = data["log_levels"];
			for (std::underlying_type_t<LogLevel> i = 0; i <= stl::underlying(LogLevel::Max); i++)
			{
				a_out.logLevels[i] = levels.get(i, true).asBool();
			}

			a_out.closeOnESC = data.get("close_on_esc", true).asBool();

			a_out.defaultExportFlags = static_cast<Data::ConfigStoreSerializationFlags>(
				data.get("default_export_flags", stl::underlying(Data::ConfigStoreSerializationFlags::kAll)).asUInt());

			a_out.font = data["font"].asString();

			if (data.isMember("font_size"))
			{
				a_out.fontSize = std::clamp(data.get("font_size", 13.0f).asFloat(), 1.0f, 140.0f);
			}

			a_out.extraGlyphs = static_cast<GlyphPresetFlags>(
				data.get("extra_glyphs", stl::underlying(GlyphPresetFlags::kNone)).asUInt());

			a_out.releaseFontData = data.get("release_font_data", true).asBool();

			a_out.selectedDefaultConfImport = static_cast<Data::DefaultConfigType>(
				data.get("sel_def_conf_import", stl::underlying(Data::DefaultConfigType::kUser)).asUInt());

			a_out.stylePreset = static_cast<UIStylePreset>(
				data.get("style_preset", stl::underlying(UIStylePreset::Dark)).asUInt());

			a_out.alpha = data.get("alpha", 1.0f).asFloat();

			if (data.isMember("bg_alpha"))
			{
				a_out.bgAlpha = data.get("bg_alpha", 1.0f).asFloat();
			}

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::UserInterface>::Create(
			const Data::SettingHolder::UserInterface& a_data,
			Json::Value&                              a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::SettingHolder::EditorPanel>   editorPanelParser(m_state);
			Parser<Data::SettingHolder::ProfileEditor> profileEditorParser(m_state);
			Parser<Data::ConfigKeyPair>                controlsParser(m_state);
			Parser<Data::SettingHolder::ImportExport>  ieParser(m_state);
			Parser<UI::UIData::UICollapsibleStates>    colStatesParser(m_state);

			editorPanelParser.Create(a_data.slotEditor, data["slot_editor"]);
			editorPanelParser.Create(a_data.customEditor, data["custom_editor"]);
			editorPanelParser.Create(a_data.transformEditor, data["transform_editor"]);

			profileEditorParser.Create(
				a_data.slotProfileEditor,
				data["slot_profile_editor"]);

			profileEditorParser.Create(
				a_data.customProfileEditor,
				data["custom_profile_editor"]);

			profileEditorParser.Create(
				a_data.transformProfileEditor,
				data["transform_profile_editor"]);

			ieParser.Create(
				a_data.importExport,
				data["import_export"]);

			colStatesParser.Create(a_data.settingsColStates, data["settings_col_states"]);
			colStatesParser.Create(a_data.statsColStates, data["stats_col_states"]);

			data["last_editor_panel"] = stl::underlying(a_data.lastPanel);

			if (a_data.openKeys)
			{
				controlsParser.Create(*a_data.openKeys, data["toggle_keys"]);
			}

			data["enable_control_lock"]    = a_data.enableControlLock;
			data["enable_freeze_time"]     = a_data.enableFreezeTime;
			data["enable_restrictions"]    = a_data.enableRestrictions;
			data["select_crosshair_actor"] = a_data.selectCrosshairActor;
			data["scale"]                  = a_data.scale;
			data["log_limit"]              = a_data.logLimit;
			data["log_show_ts"]            = a_data.logShowTimestamps;

			auto& levels = (data["log_levels"] = Json::Value(Json::ValueType::arrayValue));
			for (auto& e : a_data.logLevels)
			{
				levels.append(e);
			}

			data["close_on_esc"] = a_data.closeOnESC;

			data["default_export_flags"] = stl::underlying(a_data.defaultExportFlags.value);

			data["font"] = *a_data.font;
			if (a_data.fontSize)
			{
				data["font_size"] = *a_data.fontSize;
			}
			data["extra_glyphs"]      = stl::underlying(a_data.extraGlyphs.value);
			data["release_font_data"] = a_data.releaseFontData;

			data["sel_def_conf_import"] = stl::underlying(a_data.selectedDefaultConfImport);

			data["style_preset"] = stl::underlying(a_data.stylePreset);
			data["alpha"]        = a_data.alpha;

			if (a_data.bgAlpha)
			{
				data["bg_alpha"] = *a_data.bgAlpha;
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
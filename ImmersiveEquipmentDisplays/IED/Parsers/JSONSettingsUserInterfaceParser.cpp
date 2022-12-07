#include "pch.h"

#include "JSONActorInfoParser.h"
#include "JSONConditionalVariablesEditor.h"
#include "JSONConfigKeyPairParser.h"
#include "JSONSettingsCollapsibleStatesParser.h"
#include "JSONSettingsEditorPanelParser.h"
#include "JSONSettingsI3DIParser.h"
#include "JSONSettingsImportExport.h"
#include "JSONSettingsProfileEditorParser.h"
#include "JSONSettingsUserInterfaceParser.h"
#include "JSONSkeletonExplorerParser.h"

#include "Fonts/FontInfo.h"

#include "IED/UI/UIChildWindowID.h"

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

			Parser<Data::SettingHolder::EditorPanel>                editorPanelParser(m_state);
			Parser<Data::SettingHolder::ProfileEditor>              profileEditorParser(m_state);
			Parser<Data::ConfigKeyPair>                             controlsParser(m_state);
			Parser<Data::SettingHolder::ImportExport>               ieParser(m_state);
			Parser<Data::SettingHolder::SkeletonExplorer>           skelExp(m_state);
			Parser<Data::SettingHolder::ActorInfo>                  ainfo(m_state);
			Parser<Data::SettingHolder::ConditionalVariablesEditor> cved(m_state);
			Parser<Data::SettingHolder::I3DI>                       i3di(m_state);
			Parser<UI::UIData::UICollapsibleStates>                 colStatesParser(m_state);

			if (!editorPanelParser.Parse(
					data["slot_editor"],
					a_out.slotEditor))
			{
				return false;
			}

			if (!editorPanelParser.Parse(
					data["custom_editor"],
					a_out.customEditor))
			{
				return false;
			}

			if (!editorPanelParser.Parse(
					data["transform_editor"],
					a_out.transformEditor))
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

			if (!skelExp.Parse(
					data["skel_exp"],
					a_out.skeletonExplorer))
			{
				return false;
			}

			if (!ainfo.Parse(
					data["actor_info"],
					a_out.actorInfo))
			{
				return false;
			}

			if (!cved.Parse(
					data["cv_editor"],
					a_out.condVarEditor))
			{
				return false;
			}

			if (!cved.Parse(
					data["cv_profile_editor"],
					a_out.condVarProfileEditor))
			{
				return false;
			}

			if (!i3di.Parse(
					data["i3di"],
					a_out.i3di))
			{
				return false;
			}

			auto lastPanel = data.get("last_editor_panel", stl::underlying(UI::UIDisplayManagementEditorPanel::Slot)).asUInt();

			switch (lastPanel)
			{
			case 1:
				a_out.lastPanel = UI::UIDisplayManagementEditorPanel::Custom;
				break;
			default:
				a_out.lastPanel = UI::UIDisplayManagementEditorPanel::Slot;
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

			if (auto& keys = data["release_keys"])
			{
				if (!controlsParser.Parse(
						keys,
						*a_out.releaseLockKeys))
				{
					return false;
				}

				a_out.releaseLockKeys.mark(true);
			}

			a_out.releaseLockAlpha        = data.get("release_alpha", 0.33f).asFloat();
			a_out.releaseLockUnfreezeTime = data.get("release_unfreeze_time", false).asBool();

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

			a_out.closeOnESC            = data.get("close_on_esc", true).asBool();
			a_out.exitOnLastWindowClose = data.get("exit_on_last_window_close", true).asBool();
			a_out.showIntroBanner       = data.get("show_intro_banner", true).asBool();
			a_out.enableNotifications   = data.get("enable_notifications", false).asBool();

			const auto notificationThreshold = std::clamp(
				data.get(
						"notification_threshold",
						stl::underlying(LogLevel::Message))
					.asUInt(),
				static_cast<std::uint32_t>(LogLevel::Min),
				static_cast<std::uint32_t>(LogLevel::Max));

			a_out.notificationThreshold = static_cast<LogLevel>(notificationThreshold);

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

			if (auto& cws = data["child_window_states"])
			{
				using enum_type = std::underlying_type_t<UI::ChildWindowID>;

				for (enum_type i = 0; i < stl::underlying(UI::ChildWindowID::kMax); i++)
				{
					a_out.windowOpenStates[i] = cws[i].asBool();
				}
			}

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::UserInterface>::Create(
			const Data::SettingHolder::UserInterface& a_data,
			Json::Value&                              a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::SettingHolder::EditorPanel>                editorPanelParser(m_state);
			Parser<Data::SettingHolder::ProfileEditor>              profileEditorParser(m_state);
			Parser<Data::ConfigKeyPair>                             controlsParser(m_state);
			Parser<Data::SettingHolder::ImportExport>               ieParser(m_state);
			Parser<Data::SettingHolder::SkeletonExplorer>           skelExp(m_state);
			Parser<Data::SettingHolder::ActorInfo>                  ainfo(m_state);
			Parser<Data::SettingHolder::ConditionalVariablesEditor> cved(m_state);
			Parser<Data::SettingHolder::I3DI>                       i3di(m_state);
			Parser<UI::UIData::UICollapsibleStates>                 colStatesParser(m_state);

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

			skelExp.Create(
				a_data.skeletonExplorer,
				data["skel_exp"]);

			ainfo.Create(
				a_data.actorInfo,
				data["actor_info"]);

			cved.Create(
				a_data.condVarEditor,
				data["cv_editor"]);

			cved.Create(
				a_data.condVarProfileEditor,
				data["cv_profile_editor"]);

			i3di.Create(
				a_data.i3di,
				data["i3di"]);

			colStatesParser.Create(a_data.settingsColStates, data["settings_col_states"]);
			colStatesParser.Create(a_data.statsColStates, data["stats_col_states"]);

			data["last_editor_panel"] = stl::underlying(a_data.lastPanel);

			if (a_data.openKeys)
			{
				controlsParser.Create(*a_data.openKeys, data["toggle_keys"]);
			}

			if (a_data.releaseLockKeys)
			{
				controlsParser.Create(*a_data.releaseLockKeys, data["release_keys"]);
			}

			data["release_alpha"]         = a_data.releaseLockAlpha;
			data["release_unfreeze_time"] = a_data.releaseLockUnfreezeTime;

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

			data["close_on_esc"]              = a_data.closeOnESC;
			data["exit_on_last_window_close"] = a_data.exitOnLastWindowClose;
			data["show_intro_banner"]         = a_data.showIntroBanner;
			data["enable_notifications"]      = a_data.enableNotifications;
			data["notification_threshold"]        = stl::underlying(a_data.notificationThreshold);

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

			{
				auto& cws = (data["child_window_states"] = Json::Value(Json::ValueType::arrayValue));

				using enum_type = std::underlying_type_t<UI::ChildWindowID>;

				for (enum_type i = 0; i < stl::underlying(UI::ChildWindowID::kMax); i++)
				{
					cws[i] = a_data.windowOpenStates[i];
				}
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
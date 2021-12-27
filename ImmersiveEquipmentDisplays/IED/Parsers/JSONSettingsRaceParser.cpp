#include "pch.h"

#include "JSONConfigSexParser.h"
#include "JSONSettingsRaceParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::EditorPanelRaceSettings>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::EditorPanelRaceSettings& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::ConfigSex> sexParser(m_state);

			if (!sexParser.Parse(data, a_out.sex, version))
			{
				return false;
			}

			a_out.playableOnly = data.get("playable_only", true).asBool();
			a_out.showEditorIDs = data.get("show_editor_ids", true).asBool();
			a_out.selectCrosshairActor =
				data.get("select_crosshair_actor", true).asBool();

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::EditorPanelRaceSettings>::Create(
			const Data::SettingHolder::EditorPanelRaceSettings& a_data,
			Json::Value& a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::ConfigSex> sexParser(m_state);

			sexParser.Create(a_data.sex, data);

			data["playable_only"] = a_data.playableOnly;
			data["show_editor_ids"] = a_data.showEditorIDs;
			data["select_crosshair_actor"] = a_data.selectCrosshairActor;

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::SettingHolder::EditorPanelRaceSettings>::GetDefault(
			Data::SettingHolder::EditorPanelRaceSettings& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED
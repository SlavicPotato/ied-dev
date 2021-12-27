#include "pch.h"

#include "JSONConfigSexParser.h"
#include "JSONSettingsActorParser.h"
#include "JSONSettingsCollapsibleStatesParser.h"
#include "JSONSettingsEditorPanelParser.h"
#include "JSONSettingsRaceParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::EditorPanel>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::EditorPanel& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::SettingHolder::EditorPanelActorSettings> actorSettingsParser(m_state);
			Parser<Data::SettingHolder::EditorPanelRaceSettings> raceSettingsParser(m_state);
			Parser<UI::UIData::UICollapsibleStates> colStatesParser(m_state);
			Parser<Data::ConfigSex> sexParser(m_state);

			if (!actorSettingsParser.Parse(data["actor_config"], a_out.actorConfig))
			{
				return false;
			}

			if (!raceSettingsParser.Parse(data["race_config"], a_out.raceConfig))
			{
				return false;
			}
			
			if (!actorSettingsParser.Parse(data["npc_config"], a_out.npcConfig))
			{
				return false;
			}

			if (!sexParser.Parse(data, a_out.globalSex, version))
			{
				return false;
			}

			auto globalType = data.get("global_type", stl::underlying(Data::GlobalConfigType::Player)).asUInt();

			a_out.globalType = globalType == 1 ? Data::GlobalConfigType::NPC : Data::GlobalConfigType::Player;

			if (auto& cs = data["col_states"]; !cs.empty())
			{
				for (std::uint32_t i = 0; i < std::size(a_out.colStates); i++)
				{
					colStatesParser.Parse(cs[i], a_out.colStates[i]);
				}
			}

			a_out.sexSync = data.get("sex_sync", false).asBool();
			a_out.eoPropagation = data.get("eo_propagation", false).asBool();
			a_out.flags = data.get("common_flags", false).asUInt();

			auto lastClass = data.get("last_config_class", stl::underlying(Data::ConfigClass::Global)).asUInt();

			switch (lastClass)
			{
			case 0:
				a_out.lastConfigClass = Data::ConfigClass::Race;
				break;
			case 1:
				a_out.lastConfigClass = Data::ConfigClass::NPC;
				break;
			case 2:
				a_out.lastConfigClass = Data::ConfigClass::Actor;
				break;
			default:
				a_out.lastConfigClass = Data::ConfigClass::Global;
				break;
			}

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::EditorPanel>::Create(
			const Data::SettingHolder::EditorPanel& a_data,
			Json::Value& a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::SettingHolder::EditorPanelActorSettings> actorSettingsParser(m_state);
			Parser<Data::SettingHolder::EditorPanelRaceSettings> raceSettingsParser(m_state);
			Parser<UI::UIData::UICollapsibleStates> colStatesParser(m_state);
			Parser<Data::ConfigSex> sexParser(m_state);

			actorSettingsParser.Create(a_data.actorConfig, data["actor_config"]);
			raceSettingsParser.Create(a_data.raceConfig, data["race_config"]);
			actorSettingsParser.Create(a_data.npcConfig, data["npc_config"]);
			sexParser.Create(a_data.globalSex, data);

			data["global_type"] = stl::underlying(a_data.globalType);
			data["sex_sync"] = a_data.sexSync;
			data["eo_propagation"] = a_data.eoPropagation;
			data["common_flags"] = a_data.flags;

			data["last_config_class"] = stl::underlying(a_data.lastConfigClass);

			auto& cs = data["col_states"];
			for (std::uint32_t i = 0; i < std::size(a_data.colStates); i++)
			{
				colStatesParser.Create(a_data.colStates[i], cs[i]);
			}

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::SettingHolder::EditorPanel>::GetDefault(
			Data::SettingHolder::EditorPanel& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED
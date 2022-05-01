#include "pch.h"

#include "JSONSkeletonExplorerParser.h"

#include "JSONSettingsActorParser.h"
#include "JSONSettingsCollapsibleStatesParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::SkeletonExplorer>::Parse(
			const Json::Value&                     a_in,
			Data::SettingHolder::SkeletonExplorer& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::SettingHolder::EditorPanelActorSettings> actorSettingsParser(m_state);
			Parser<UI::UIData::UICollapsibleStates>               colStatesParser(m_state);

			if (!actorSettingsParser.Parse(data["act_settings"], a_out.actorSettings))
			{
				return false;
			}

			if (!colStatesParser.Parse(data["col_states"], a_out.colStates))
			{
				return false;
			}

			a_out.filterShowChildNodes = data.get("show_child_nodes", false).asBool();
			a_out.firstPerson          = data.get("first_person", false).asBool();
			a_out.showLoadedSkeleton   = data.get("loaded", true).asBool();

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::SkeletonExplorer>::Create(
			const Data::SettingHolder::SkeletonExplorer& a_data,
			Json::Value&                                 a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::SettingHolder::EditorPanelActorSettings> actorSettingsParser(m_state);
			Parser<UI::UIData::UICollapsibleStates>               colStatesParser(m_state);

			actorSettingsParser.Create(a_data.actorSettings, data["act_settings"]);
			colStatesParser.Create(a_data.colStates, data["col_states"]);

			data["show_child_nodes"] = a_data.filterShowChildNodes;
			data["first_person"]     = a_data.firstPerson;
			data["loaded"]           = a_data.showLoadedSkeleton;

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
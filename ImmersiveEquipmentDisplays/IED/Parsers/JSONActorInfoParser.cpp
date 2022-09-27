#include "pch.h"

#include "JSONActorInfoParser.h"

#include "JSONSettingsActorParser.h"
#include "JSONSettingsCollapsibleStatesParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::ActorInfo>::Parse(
			const Json::Value&              a_in,
			Data::SettingHolder::ActorInfo& a_out) const
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

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::ActorInfo>::Create(
			const Data::SettingHolder::ActorInfo& a_data,
			Json::Value&                          a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::SettingHolder::EditorPanelActorSettings> actorSettingsParser(m_state);
			Parser<UI::UIData::UICollapsibleStates>               colStatesParser(m_state);

			actorSettingsParser.Create(a_data.actorSettings, data["act_settings"]);
			colStatesParser.Create(a_data.colStates, data["col_states"]);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
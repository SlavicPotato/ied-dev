#include "pch.h"

#include "JSONConditionalVariablesEditor.h"

#include "JSONSettingsCollapsibleStatesParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::ConditionalVariablesEditor>::Parse(
			const Json::Value&                               a_in,
			Data::SettingHolder::ConditionalVariablesEditor& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<UI::UIData::UICollapsibleStates> colStatesParser(m_state);

			if (!colStatesParser.Parse(data["col_states"], a_out.colStates))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::ConditionalVariablesEditor>::Create(
			const Data::SettingHolder::ConditionalVariablesEditor& a_data,
			Json::Value&                                           a_out) const
		{
			auto& data = a_out["data"];

			Parser<UI::UIData::UICollapsibleStates> colStatesParser(m_state);

			colStatesParser.Create(a_data.colStates, data["col_states"]);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
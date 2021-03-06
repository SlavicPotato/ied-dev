#include "pch.h"

#include "JSONConfigSexParser.h"
#include "JSONSettingsCollapsibleStatesParser.h"
#include "JSONSettingsProfileEditorParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::SettingHolder::ProfileEditor>::Parse(
			const Json::Value&                  a_in,
			Data::SettingHolder::ProfileEditor& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::ConfigSex>                 sexParser(m_state);
			Parser<UI::UIData::UICollapsibleStates> colStatesParser(m_state);

			if (!sexParser.Parse(data, a_out.sex, version))
			{
				return false;
			}

			if (!colStatesParser.Parse(data["col_states"], a_out.colStates))
			{
				return false;
			}

			a_out.sexSync       = data.get("sex_sync", false).asBool();
			a_out.eoPropagation = data.get("eo_propagation", false).asBool();

			return true;
		}

		template <>
		void Parser<Data::SettingHolder::ProfileEditor>::Create(
			const Data::SettingHolder::ProfileEditor& a_data,
			Json::Value&                              a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::ConfigSex>                 sexParser(m_state);
			Parser<UI::UIData::UICollapsibleStates> colStatesParser(m_state);

			sexParser.Create(a_data.sex, data);
			colStatesParser.Create(a_data.colStates, data["col_states"]);

			data["sex_sync"]       = a_data.sexSync;
			data["eo_propagation"] = a_data.eoPropagation;

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
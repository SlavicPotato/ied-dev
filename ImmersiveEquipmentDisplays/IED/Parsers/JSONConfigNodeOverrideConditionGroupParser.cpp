#include "pch.h"

#include "JSONConfigNodeOverrideConditionGroupParser.h"
#include "JSONConfigNodeOverrideConditionListParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configNodeOverrideConditionGroup_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideConditionGroup_t& a_out) const
		{
			JSON_PARSE_VERSION()

			Parser<Data::configNodeOverrideConditionList_t> lparser(m_state);

			auto& data = a_in["data"];

			if (!lparser.Parse(data["cond"], a_out.conditions))
			{
				return false;
			}

			a_out.flags = static_cast<Data::NodeOverrideConditionGroupFlags>(
				data.get("flags", stl::underlying(Data::NodeOverrideConditionGroupFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideConditionGroup_t>::Create(
			const Data::configNodeOverrideConditionGroup_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configNodeOverrideConditionList_t> lparser(m_state);

			lparser.Create(a_data.conditions, data["cond"]);

			data["flags"] = stl::underlying(a_data.flags.value);

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::configNodeOverrideConditionGroup_t>::GetDefault(
			Data::configNodeOverrideConditionGroup_t& a_out) const
		{
		}
	}
}
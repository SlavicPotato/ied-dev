#include "pch.h"

#include "JSONConfigConditionalVariableParser.h"

#include "JSONConfigConditionalVariableValueDataParser.h"
#include "JSONConfigConditionalVariablesListParser.h"
#include "JSONEquipmentOverrideConditionListParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configConditionalVariable_t>::Parse(
			const Json::Value&                 a_in,
			Data::configConditionalVariable_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			a_out.flags = data.get("flags", stl::underlying(Data::ConditionalVariableFlags::kNone)).asUInt();
			a_out.desc  = data["desc"].asString();

			if (auto& v = data["cond"])
			{
				Parser<Data::equipmentOverrideConditionList_t> parser(m_state);

				if (!parser.Parse(v, a_out.conditions))
				{
					return false;
				}
			}

			if (auto& v = data["grp"])
			{
				Parser<Data::configConditionalVariablesList_t> parser(m_state);

				if (!parser.Parse(v, a_out.group))
				{
					return false;
				}
			}

			Parser<Data::configConditionalVariableValueData_t> cvdvparser(m_state);

			if (!cvdvparser.Parse(data["vdata"], a_out.value))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::configConditionalVariable_t>::Create(
			const Data::configConditionalVariable_t& a_data,
			Json::Value&                             a_out) const
		{
			auto& data = a_out["data"];

			data["flags"] = a_data.flags.underlying();
			data["desc"]  = *a_data.desc;

			if (!a_data.conditions.empty())
			{
				Parser<Data::equipmentOverrideConditionList_t> parser(m_state);

				parser.Create(a_data.conditions, data["cond"]);
			}

			if (!a_data.group.empty())
			{
				Parser<Data::configConditionalVariablesList_t> parser(m_state);

				parser.Create(a_data.group, data["grp"]);
			}

			Parser<Data::configConditionalVariableValueData_t> cvdvparser(m_state);

			cvdvparser.Create(a_data.value, data["vdata"]);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
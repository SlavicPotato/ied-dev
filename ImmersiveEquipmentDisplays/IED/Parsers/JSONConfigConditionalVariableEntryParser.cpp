#include "pch.h"

#include "JSONConfigConditionalVariableEntryParser.h"

#include "JSONConfigConditionalVariableEntryListParser.h"
#include "JSONConfigConditionalVariablesListParser.h"
#include "JSONEquipmentOverrideConditionListParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configConditionalVariablesEntry_t>::Parse(
			const Json::Value&                       a_in,
			Data::configConditionalVariablesEntry_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			a_out.flags = data.get("flags", stl::underlying(Data::ConditionalVariablesEntryFlags::kNone)).asUInt();
			a_out.desc  = data.get("desc", "").asString();

			if (auto& v = data["vars"])
			{
				Parser<Data::configConditionalVariablesList_t> parser(m_state);

				if (!parser.Parse(v, a_out.vars))
				{
					return false;
				}
			}

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
				Parser<Data::configConditionalVariablesEntryList_t> parser(m_state);

				if (!parser.Parse(v, a_out.group))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configConditionalVariablesEntry_t>::Create(
			const Data::configConditionalVariablesEntry_t& a_data,
			Json::Value&                                   a_out) const
		{
			auto& data = a_out["data"];

			data["flags"] = a_data.flags.underlying();
			data["desc"]  = *a_data.desc;

			if (!a_data.vars.empty())
			{
				Parser<Data::configConditionalVariablesList_t> parser(m_state);

				parser.Create(a_data.vars, data["vars"]);
			}

			if (!a_data.conditions.empty())
			{
				Parser<Data::equipmentOverrideConditionList_t> parser(m_state);

				parser.Create(a_data.conditions, data["cond"]);
			}

			if (!a_data.group.empty())
			{
				Parser<Data::configConditionalVariablesEntryList_t> parser(m_state);

				parser.Create(a_data.group, data["grp"]);
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
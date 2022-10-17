#include "pch.h"

#include "JSONConfigConditionalVariableEntryParser.h"

#include "JSONConditionalVariableStorageParser.h"
#include "JSONConfigConditionalVariablesListParser.h"


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

			Parser<conditionalVariableStorage_t> vsparser(m_state);

			if (!vsparser.Parse(data["defval"], a_out.defaultValue))
			{
				return false;
			}

			if (auto& v = data["vars"])
			{
				Parser<Data::configConditionalVariablesList_t> parser(m_state);

				if (!parser.Parse(v, a_out.vars))
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

			Parser<conditionalVariableStorage_t> vsparser(m_state);

			vsparser.Create(a_data.defaultValue, data["defval"]);

			if (!a_data.vars.empty())
			{
				Parser<Data::configConditionalVariablesList_t> parser(m_state);

				parser.Create(a_data.vars, data["vars"]);
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
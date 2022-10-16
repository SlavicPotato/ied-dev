#include "pch.h"

#include "JSONConfigConditionalVariablesHolderParser.h"

#include "JSONConfigConditionalVariableEntryListParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configConditionalVariablesHolder_t>::Parse(
			const Json::Value&                        a_in,
			Data::configConditionalVariablesHolder_t& a_out) const
		{
			JSON_PARSE_VERSION();

			Parser<Data::configConditionalVariablesEntryList_t> parser(m_state);

			auto& data = a_in["data"];

			a_out.flags = data.get("flags", stl::underlying(Data::ConditionalVariablesHolderFlags::kNone)).asUInt();

			if (auto& v = data["data"])
			{
				if (!parser.Parse(v, a_out.data))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configConditionalVariablesHolder_t>::Create(
			const Data::configConditionalVariablesHolder_t& a_data,
			Json::Value&                                    a_out) const
		{
			auto& data = a_out["data"];

			Parser<Data::configConditionalVariablesEntryList_t> parser(m_state);

			data["flags"] = a_data.flags.underlying();

			if (!a_data.data.empty())
			{
				parser.Create(a_data.data, data["data"]);
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
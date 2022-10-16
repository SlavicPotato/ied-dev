#include "pch.h"

#include "JSONConfigConditionalVariableParser.h"

#include "JSONConditionalVariableStorageParser.h"

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

			Parser<conditionalVariableStorage_t> cvsparser(m_state);

			auto& data = a_in["data"];

			a_out.flags = data.get("flags", stl::underlying(Data::ConditionalVariableFlags::kNone)).asUInt();
			a_out.name  = data["name"].asString();

			if (!cvsparser.Parse(data["stor"], a_out.storage))
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
			Parser<conditionalVariableStorage_t> cvsparser(m_state);

			auto& data = a_out["data"];

			data["flags"] = a_data.flags.underlying();
			data["name"]  = *a_data.name;

			cvsparser.Create(a_data.storage, data["stor"]);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
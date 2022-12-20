#include "pch.h"

#include "JSONConfigConditionalVariableValueDataParser.h"

#include "JSONConditionalVariableStorageParser.h"
#include "JSONConfigLastEquippedParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configConditionalVariableValueData_t>::Parse(
			const Json::Value&                          a_in,
			Data::configConditionalVariableValueData_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			Parser<conditionalVariableStorage_t> cvsparser(m_state);

			if (!cvsparser.Parse(data["value"], a_out.value))
			{
				return false;
			}

			Parser<Data::configLastEquipped_t> leqpparser(m_state);

			if (!leqpparser.Parse(data["leqp"], a_out.lastEquipped))
			{
				return false;
			}

			a_out.flags = data.get(
								  "flags",
								  stl::underlying(Data::ConditionalVariableValueDataFlags::kNone))
			                  .asUInt();

			return true;
		}

		template <>
		void Parser<Data::configConditionalVariableValueData_t>::Create(
			const Data::configConditionalVariableValueData_t& a_data,
			Json::Value&                                      a_out) const
		{
			Create(a_data, a_out, static_cast<std::uint32_t>(a_data.value.type));
		}
		
		template <>
		void Parser<Data::configConditionalVariableValueData_t>::Create(
			const Data::configConditionalVariableValueData_t& a_data,
			Json::Value&                                      a_out,
			std::uint32_t                                     a_type) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<conditionalVariableStorage_t> cvsparser(m_state);

			cvsparser.Create(a_data.value, data["value"], a_type);

			Parser<Data::configLastEquipped_t> leqpparser(m_state);

			leqpparser.Create(a_data.lastEquipped, data["leqp"]);

			data["flags"] = a_data.flags.underlying();

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
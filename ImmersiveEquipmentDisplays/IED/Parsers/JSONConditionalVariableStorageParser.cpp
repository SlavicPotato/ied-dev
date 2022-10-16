#include "pch.h"

#include "JSONConditionalVariableStorageParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<conditionalVariableStorage_t>::Parse(
			const Json::Value&            a_in,
			conditionalVariableStorage_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			a_out.type = static_cast<ConditionalVariableType>(data["t"].asUInt());

			switch (a_out.type)
			{
			case ConditionalVariableType::kInt32:
				a_out.i32 = data["v"].asInt();
				break;
			case ConditionalVariableType::kFloat:
				a_out.f32 = data["v"].asFloat();
				break;
			default:
				Error("%s: invalid type %u", __FUNCTION__, a_out.type);
				return false;
			}

			return true;
		}

		template <>
		void Parser<conditionalVariableStorage_t>::Create(
			const conditionalVariableStorage_t& a_data,
			Json::Value&                        a_out) const
		{
			auto& data = a_out["data"];

			data["t"] = static_cast<std::uint32_t>(a_data.type);

			switch (a_data.type)
			{
			case ConditionalVariableType::kInt32:
				data["v"] = a_data.i32;
				break;
			case ConditionalVariableType::kFloat:
				data["v"] = a_data.f32;
				break;
			default:
				throw std::exception("unknown type");
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
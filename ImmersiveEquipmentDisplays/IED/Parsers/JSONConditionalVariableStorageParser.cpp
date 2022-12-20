#include "pch.h"

#include "JSONConditionalVariableStorageParser.h"

#include "JSONConfigCachedFormParser.h"

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
			case ConditionalVariableType::kForm:
				{
					Parser<Data::configCachedForm_t> parser(m_state);

					if (!parser.Parse(data["v"], a_out.form))
					{
						return false;
					}
				}
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
			Create(a_data, a_out, static_cast<std::uint32_t>(a_data.type));
		}

		template <>
		void Parser<conditionalVariableStorage_t>::Create(
			const conditionalVariableStorage_t& a_data,
			Json::Value&                        a_out,
			std::uint32_t                       a_type) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			data["t"] = a_type;

			switch (static_cast<ConditionalVariableType>(a_type))
			{
			case ConditionalVariableType::kInt32:
				data["v"] = a_data.i32;
				break;
			case ConditionalVariableType::kFloat:
				data["v"] = a_data.f32;
				break;
			case ConditionalVariableType::kForm:
				{
					Parser<Data::configCachedForm_t> parser(m_state);

					parser.Create(a_data.form, data["v"]);
				}
				break;
			default:
				throw std::exception("unknown type");
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
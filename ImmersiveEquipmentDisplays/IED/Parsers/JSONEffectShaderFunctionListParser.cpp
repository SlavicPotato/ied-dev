#include "pch.h"

#include "JSONEffectShaderFunctionListParser.h"
#include "JSONEffectShaderFunctionParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configEffectShaderFunctionList_t>::Parse(
			const Json::Value&                      a_in,
			Data::configEffectShaderFunctionList_t& a_out) const
		{
			JSON_PARSE_VERSION()

			Parser<Data::configEffectShaderFunction_t> parser(m_state);

			auto& data = a_in["data"];

			for (auto& e : data)
			{
				if (!parser.Parse(e, a_out.emplace_back(), version))
				{
					Error("%s: failed parsing effect shader function", __FUNCTION__);
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configEffectShaderFunctionList_t>::Create(
			const Data::configEffectShaderFunctionList_t& a_data,
			Json::Value&                                  a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::configEffectShaderFunction_t> parser(m_state);

			for (auto& e : a_data)
			{
				parser.Create(e, data.append(Json::Value()));
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
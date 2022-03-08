#include "pch.h"

#include "JSONEffectShaderListParser.h"
#include "JSONEffectShaderHolderParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::effectShaderList_t>::Parse(
			const Json::Value&        a_in,
			Data::effectShaderList_t& a_out) const
		{
			JSON_PARSE_VERSION();

			auto& data = a_in["data"];

			if (auto& list = data["list"])
			{
				Parser<Data::configEffectShaderHolder_t> parser(m_state);

				for (auto& e : list)
				{
					if (!parser.Parse(e, a_out.data.emplace_back()))
					{
						return false;
					}
				}
			}

			return true;
		}

		template <>
		void Parser<Data::effectShaderList_t>::Create(
			const Data::effectShaderList_t& a_data,
			Json::Value&                    a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			if (!a_data.data.empty())
			{
				Parser<Data::configEffectShaderHolder_t> parser(m_state);

				auto& out = (data["list"] = Json::Value(Json::ValueType::arrayValue));

				for (auto& e : a_data.data)
				{
					parser.Create(e, out.append(Json::Value()));
				}
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
#include "pch.h"

#include "JSONConfigNodeOverridePlacementOverrideListParser.h"
#include "JSONConfigNodeOverridePlacementOverrideParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configNodeOverridePlacementOverrideList_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverridePlacementOverrideList_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::configNodeOverridePlacementOverride_t> parser(m_state);

			for (auto& e : data)
			{
				Data::configNodeOverridePlacementOverride_t tmp;

				if (!parser.Parse(e, tmp, version))
				{
					continue;
				}

				a_out.emplace_back(std::move(tmp));
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverridePlacementOverrideList_t>::Create(
			const Data::configNodeOverridePlacementOverrideList_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::configNodeOverridePlacementOverride_t> parser(m_state);

			for (auto& e : a_data)
			{
				Json::Value v;

				parser.Create(e, v);

				data.append(std::move(v));
			}

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::configNodeOverridePlacementOverrideList_t>::GetDefault(
			Data::configNodeOverridePlacementOverrideList_t& a_out) const
		{
		}
	}
}
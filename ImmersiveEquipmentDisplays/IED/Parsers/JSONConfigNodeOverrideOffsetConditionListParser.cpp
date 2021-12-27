#include "pch.h"

#include "JSONConfigNodeOverrideOffsetConditionListParser.h"
#include "JSONConfigNodeOverrideOffsetConditionParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configNodeOverrideConditionList_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideConditionList_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::configNodeOverrideCondition_t> mparser(m_state);

			for (auto& e : data)
			{
				Data::configNodeOverrideCondition_t tmp;

				if (!mparser.Parse(e, tmp, version))
				{
					continue;
				}

				a_out.emplace_back(std::move(tmp));
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideConditionList_t>::Create(
			const Data::configNodeOverrideConditionList_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::configNodeOverrideCondition_t> mparser(m_state);

			for (auto& e : a_data)
			{
				Json::Value v;

				mparser.Create(e, v);

				data.append(std::move(v));
			}

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::configNodeOverrideConditionList_t>::GetDefault(
			Data::configNodeOverrideConditionList_t& a_out) const
		{
		}
	}
}
#include "pch.h"

#include "JSONConfigNodeOverrideConditionListParser.h"
#include "JSONConfigNodeOverrideConditionParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 2;

		template <>
		bool Parser<Data::configNodeOverrideConditionList_t>::Parse(
			const Json::Value&                       a_in,
			Data::configNodeOverrideConditionList_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::configNodeOverrideCondition_t> mparser(m_state);

			for (auto& e : data)
			{
				if (!mparser.Parse(e, a_out.emplace_back(), version))
				{
					Error("%s: failed parsing node override condition", __FUNCTION__);
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideConditionList_t>::Create(
			const Data::configNodeOverrideConditionList_t& a_data,
			Json::Value&                                   a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::configNodeOverrideCondition_t> mparser(m_state);

			for (auto& e : a_data)
			{
				mparser.Create(e, data.append(Json::Value()));
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
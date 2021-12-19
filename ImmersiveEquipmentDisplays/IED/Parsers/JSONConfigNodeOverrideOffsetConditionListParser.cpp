#include "pch.h"

#include "JSONConfigNodeOverrideOffsetConditionListParser.h"
#include "JSONConfigNodeOverrideOffsetConditionParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideConditionList_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideConditionList_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			auto& data = a_in["data"];

			Parser<Data::configNodeOverrideCondition_t> mparser;

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

			Parser<Data::configNodeOverrideCondition_t> mparser;

			for (auto& e : a_data)
			{
				Json::Value v;

				mparser.Create(e, v);

				data.append(std::move(v));
			}

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::configNodeOverrideConditionList_t>::GetDefault(
			Data::configNodeOverrideConditionList_t& a_out) const
		{
		}
	}
}
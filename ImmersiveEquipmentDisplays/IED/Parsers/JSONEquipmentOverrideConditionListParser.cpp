#include "pch.h"

#include "JSONEquipmentOverrideConditionListParser.h"
#include "JSONEquipmentOverrideConditionParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverrideConditionList_t>::Parse(
			const Json::Value& a_in,
			Data::equipmentOverrideConditionList_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			Parser<Data::equipmentOverrideCondition_t> parser(m_state);

			auto& data = a_in["data"];

			for (auto& e : data)
			{
				Data::equipmentOverrideCondition_t tmp;

				if (!parser.Parse(e, tmp, version))
				{
					Error("%s: failed parsing equipment override condition", __FUNCTION__);
					continue;
				}

				a_out.emplace_back(std::move(tmp));
			}

			return true;
		}

		template <>
		void Parser<Data::equipmentOverrideConditionList_t>::Create(
			const Data::equipmentOverrideConditionList_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::equipmentOverrideCondition_t> parser(m_state);

			for (auto& e : a_data)
			{
				Json::Value tmp;

				parser.Create(e, tmp);

				data.append(std::move(tmp));
			}

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::equipmentOverrideConditionList_t>::GetDefault(Data::equipmentOverrideConditionList_t& a_out) const
		{}
	}
}
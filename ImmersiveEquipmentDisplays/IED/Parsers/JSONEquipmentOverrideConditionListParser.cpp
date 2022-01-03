#include "pch.h"

#include "JSONEquipmentOverrideConditionListParser.h"
#include "JSONEquipmentOverrideConditionParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::equipmentOverrideConditionList_t>::Parse(
			const Json::Value& a_in,
			Data::equipmentOverrideConditionList_t& a_out) const
		{
			JSON_PARSE_VERSION()

			Parser<Data::equipmentOverrideCondition_t> parser(m_state);

			auto& data = a_in["data"];

			for (auto& e : data)
			{
				if (!parser.Parse(e, a_out.emplace_back(), version))
				{
					Error("%s: failed parsing equipment override condition", __FUNCTION__);
					return false;
				}
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
				parser.Create(e, data.append(Json::Value()));
			}

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::equipmentOverrideConditionList_t>::GetDefault(
			Data::equipmentOverrideConditionList_t& a_out) const
		{}
	}
}
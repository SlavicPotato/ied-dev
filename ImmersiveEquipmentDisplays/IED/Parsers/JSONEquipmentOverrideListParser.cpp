#include "pch.h"

#include "JSONEquipmentOverrideListParser.h"
#include "JSONEquipmentOverrideParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::equipmentOverrideList_t>::Parse(
			const Json::Value& a_in,
			Data::equipmentOverrideList_t& a_out) const
		{
			JSON_PARSE_VERSION()

			Parser<Data::equipmentOverride_t> parser(m_state);

			auto& data = a_in["data"];

			for (auto& e : data)
			{
				if (!parser.Parse(e, a_out.emplace_back(), version))
				{
					Error("%s: failed parsing equipment override entry", __FUNCTION__);
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::equipmentOverrideList_t>::Create(
			const Data::equipmentOverrideList_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::equipmentOverride_t> parser(m_state);

			for (auto& e : a_data)
			{
				parser.Create(e, data.append(Json::Value()));
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
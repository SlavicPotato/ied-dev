#include "pch.h"

#include "JSONEquipmentOverrideListParser.h"
#include "JSONEquipmentOverrideParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverrideList_t>::Parse(
			const Json::Value& a_in,
			Data::equipmentOverrideList_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			Parser<Data::equipmentOverride_t> parser(m_state);

			auto& data = a_in["data"];

			for (auto& e : data)
			{
				Data::equipmentOverride_t tmp;

				if (!parser.Parse(e, tmp, version))
				{
					Error("%s: failed parsing armor override entry", __FUNCTION__);
					continue;
				}

				a_out.emplace_back(std::move(tmp));
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
				Json::Value tmp;

				parser.Create(e, tmp);

				data.append(std::move(tmp));
			}

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::equipmentOverrideList_t>::GetDefault(Data::equipmentOverrideList_t& a_out) const
		{}
	}
}
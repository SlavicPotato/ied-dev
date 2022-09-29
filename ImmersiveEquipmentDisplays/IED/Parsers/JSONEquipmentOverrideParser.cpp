#include "pch.h"

#include "JSONEquipmentOverrideParser.h"

#include "JSONConfigBaseValuesParser.h"
#include "JSONEquipmentOverrideConditionListParser.h"
#include "JSONEquipmentOverrideListParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverride_t>::Parse(
			const Json::Value&         a_in,
			Data::equipmentOverride_t& a_out,
			const std::uint32_t        a_version) const
		{
			Parser<Data::configBaseValues_t> bvParser(m_state);

			if (!bvParser.Parse(a_in["config"], a_out, a_version))
			{
				return false;
			}

			a_out.eoFlags = a_in.get("flags", stl::underlying(Data::EquipmentOverrideFlags::kNone)).asUInt();

			if (auto& matches = a_in["matches"])
			{
				Parser<Data::equipmentOverrideConditionList_t> mlParser(m_state);

				if (!mlParser.Parse(matches, a_out.conditions))
				{
					return false;
				}
			}

			if (auto& desc = a_in["desc"])
			{
				a_out.description = desc.asString();
			}

			if (auto& group = a_in["group"])
			{
				Parser<Data::equipmentOverrideList_t> olparser(m_state);

				if (!olparser.Parse(group, a_out.group))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::equipmentOverride_t>::Create(
			const Data::equipmentOverride_t& a_data,
			Json::Value&                     a_out) const
		{
			Parser<Data::configBaseValues_t> bvParser(m_state);

			bvParser.Create(a_data, a_out["config"]);

			a_out["flags"] = stl::underlying(a_data.eoFlags.value);

			if (!a_data.conditions.empty())
			{
				Parser<Data::equipmentOverrideConditionList_t> mlParser(m_state);

				mlParser.Create(a_data.conditions, a_out["matches"]);
			}

			if (!a_data.description.empty())
			{
				a_out["desc"] = a_data.description;
			}

			if (!a_data.group.empty())
			{
				Parser<Data::equipmentOverrideList_t> olparser(m_state);

				olparser.Create(a_data.group, a_out["group"]);
			}
		}

	}
}
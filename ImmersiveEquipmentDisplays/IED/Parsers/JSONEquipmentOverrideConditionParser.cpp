#include "pch.h"

#include "JSONConfigBaseValuesParser.h"
#include "JSONConfigCachedFormParser.h"
#include "JSONEquipmentOverrideConditionParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverrideCondition_t>::Parse(
			const Json::Value& a_in,
			Data::equipmentOverrideCondition_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Game::FormID> formParser;
			Parser<Data::configCachedForm_t> cachedFormParser;

			formParser.Parse(a_in["form"], a_out.form, 1u);
			cachedFormParser.Parse(a_in["keyword"], a_out.keyword, 1u);

			a_out.slot = static_cast<Data::ObjectSlotExtra>(
				a_in.get("type", stl::underlying(Data::ObjectSlotExtra::kNone)).asUInt());

			a_out.flags = static_cast<Data::EquipmentOverrideConditionFlags>(
				a_in.get("flags", 0).asUInt());

			return true;
		}

		template <>
		void Parser<Data::equipmentOverrideCondition_t>::Create(
			const Data::equipmentOverrideCondition_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Game::FormID> formParser;
			Parser<Data::configCachedForm_t> cachedFormParser;

			if (a_data.form)
			{
				formParser.Create(a_data.form, a_out["form"]);
			}

			if (a_data.keyword.get_id())
			{
				cachedFormParser.Create(a_data.keyword, a_out["keyword"]);
			}

			a_out["type"] = stl::underlying(a_data.slot);
			a_out["flags"] = stl::underlying(a_data.flags.value);
		}

		template <>
		void Parser<Data::equipmentOverrideCondition_t>::GetDefault(Data::equipmentOverrideCondition_t& a_out) const
		{}
	}
}
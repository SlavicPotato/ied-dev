#include "pch.h"

#include "JSONConfigBaseValuesParser.h"
#include "JSONConfigCachedFormParser.h"
#include "JSONEquipmentOverrideConditionGroupParser.h"
#include "JSONEquipmentOverrideConditionParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverrideCondition_t>::Parse(
			const Json::Value&                  a_in,
			Data::equipmentOverrideCondition_t& a_out,
			const std::uint32_t                 a_version) const
		{
			Parser<Game::FormID>             formParser(m_state);
			Parser<Data::configCachedForm_t> cachedFormParser(m_state);

			if (auto& v = a_in["form"])
			{
				if (!cachedFormParser.Parse(v, a_out.form))
				{
					SetHasErrors();
					Error("%s: failed to parse form ID", __FUNCTION__);
				}
			}

			if (auto& v = a_in["keyword"])
			{
				if (!cachedFormParser.Parse(v, a_out.keyword))
				{
					SetHasErrors();
					Error("%s: failed to parse keyword form ID", __FUNCTION__);
				}
			}

			a_out.slot = static_cast<Data::ObjectSlotExtra>(
				a_in.get("type", stl::underlying(Data::ObjectSlotExtra::kNone)).asUInt());

			a_out.ui32a = a_in.get("bslot", static_cast<std::uint32_t>(-1)).asUInt();
			a_out.f32a  = a_in.get("f32a", 0.0f).asFloat();

			a_out.flags = static_cast<Data::EquipmentOverrideConditionFlags>(
				a_in.get("flags", stl::underlying(Data::EquipmentOverrideConditionFlags::kNone)).asUInt());

			Parser<Data::equipmentOverrideConditionGroup_t> gparser(m_state);

			if (!gparser.Parse(a_in["group"], a_out.group))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::equipmentOverrideCondition_t>::Create(
			const Data::equipmentOverrideCondition_t& a_data,
			Json::Value&                              a_out) const
		{
			Parser<Game::FormID>             formParser(m_state);
			Parser<Data::configCachedForm_t> cachedFormParser(m_state);

			if (a_data.form.get_id())
			{
				cachedFormParser.Create(a_data.form, a_out["form"]);
			}

			if (a_data.keyword.get_id())
			{
				cachedFormParser.Create(a_data.keyword, a_out["keyword"]);
			}

			a_out["type"]  = stl::underlying(a_data.slot);
			a_out["bslot"] = a_data.ui32a;
			a_out["f32a"]  = a_data.f32a;

			a_out["flags"] = stl::underlying(a_data.flags.value);

			Parser<Data::equipmentOverrideConditionGroup_t> gparser(m_state);

			gparser.Create(a_data.group, a_out["group"]);
		}

	}
}
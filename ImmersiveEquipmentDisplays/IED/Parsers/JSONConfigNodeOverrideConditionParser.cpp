#include "pch.h"

#include "JSONConfigCachedFormParser.h"
#include "JSONConfigNodeOverrideConditionGroupParser.h"
#include "JSONConfigNodeOverrideConditionParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideCondition_t>::Parse(
			const Json::Value&                   a_in,
			Data::configNodeOverrideCondition_t& a_out,
			const std::uint32_t                  a_version) const
		{
			Parser<Data::configCachedForm_t> fparser(m_state);

			a_out.flags = static_cast<Data::NodeOverrideConditionFlags>(
				a_in.get("flags", stl::underlying(Data::NodeOverrideConditionFlags::kNone)).asUInt());

			if (auto& node = a_in["node"])
			{
				a_out.s0 = node.asString();
			}

			if (auto& v = a_in["form"])
			{
				if (!fparser.Parse(v, a_out.form))
				{
					SetHasErrors();
					Error("%s: failed to parse form ID", __FUNCTION__);
				}
			}
			
			if (auto& v = a_in["form2"])
			{
				if (!fparser.Parse(v, a_out.form2))
				{
					SetHasErrors();
					Error("%s: failed to parse form ID (2)", __FUNCTION__);
				}
			}

			if (auto& v = a_in["kw"])
			{
				if (!fparser.Parse(v, a_out.keyword))
				{
					SetHasErrors();
					Error("%s: failed to parse keyword form ID", __FUNCTION__);
				}
			}

			a_out.ui32a    = a_in.get("bip", static_cast<std::uint32_t>(-1)).asUInt();
			a_out.typeSlot = static_cast<Data::ObjectSlotExtra>(
				a_in.get("type", stl::underlying(Data::ObjectSlotExtra::kNone)).asUInt());

			a_out.f32a  = a_in.get("f32a", 0.0f).asFloat();
			a_out.ui32b = a_in.get("ui32b", 0).asUInt();
			a_out.i32a  = a_in.get("i32a", 0).asInt();
			a_out.ui64a = a_in.get("ui64a", 0).asUInt64();
			a_out.ui32c = a_in.get("ui32c", 0).asUInt();

			Parser<Data::configNodeOverrideConditionGroup_t> gparser(m_state);

			if (!gparser.Parse(a_in["group"], a_out.group))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideCondition_t>::Create(
			const Data::configNodeOverrideCondition_t& a_data,
			Json::Value&                               a_out) const
		{
			Parser<Data::configCachedForm_t> fparser(m_state);

			a_out["flags"] = stl::underlying(a_data.flags.value);

			if (!a_data.s0.empty())
			{
				a_out["node"] = *a_data.s0;
			}

			if (a_data.form.get_id())
			{
				fparser.Create(a_data.form, a_out["form"]);
			}
			
			if (a_data.form2.get_id())
			{
				fparser.Create(a_data.form2, a_out["form2"]);
			}

			if (a_data.keyword.get_id())
			{
				fparser.Create(a_data.keyword, a_out["kw"]);
			}

			a_out["bip"]   = a_data.ui32a;
			a_out["type"]  = stl::underlying(a_data.typeSlot);
			a_out["f32a"]  = a_data.f32a;
			a_out["ui32b"] = a_data.ui32b;
			a_out["i32a"]  = a_data.i32a;
			a_out["ui64a"] = a_data.ui64a;
			a_out["ui32c"] = a_data.ui32c;

			Parser<Data::configNodeOverrideConditionGroup_t> gparser(m_state);

			gparser.Create(a_data.group, a_out["group"]);
		}

	}
}
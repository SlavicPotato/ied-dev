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
			const Json::Value& a_in,
			Data::configNodeOverrideCondition_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configCachedForm_t> fparser(m_state);

			a_out.flags = static_cast<Data::NodeOverrideConditionFlags>(
				a_in.get("flags", stl::underlying(Data::NodeOverrideConditionFlags::kNone)).asUInt());

			a_out.node = a_in["node"].asString();

			if (auto& v = a_in["form"])
			{
				if (!fparser.Parse(v, a_out.form))
				{
					SetHasErrors();
					Error("%s: failed to parse form ID", __FUNCTION__);
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

			a_out.ui32a = a_in.get("bip", static_cast<std::uint32_t>(-1)).asUInt();
			a_out.typeSlot = static_cast<Data::ObjectSlotExtra>(
				a_in.get("type", stl::underlying(Data::ObjectSlotExtra::kNone)).asUInt());

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
			Json::Value& a_out) const
		{
			Parser<Data::configCachedForm_t> fparser(m_state);

			a_out["flags"] = stl::underlying(a_data.flags.value);
			a_out["node"] = *a_data.node;

			fparser.Create(a_data.form, a_out["form"]);
			fparser.Create(a_data.keyword, a_out["kw"]);

			a_out["bip"] = a_data.ui32a;
			a_out["type"] = stl::underlying(a_data.typeSlot);

			Parser<Data::configNodeOverrideConditionGroup_t> gparser(m_state);

			gparser.Create(a_data.group, a_out["group"]);
		}

		template <>
		void Parser<Data::configNodeOverrideCondition_t>::GetDefault(
			Data::configNodeOverrideCondition_t& a_out) const
		{
		}
	}
}
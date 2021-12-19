#include "pch.h"

#include "JSONConfigNodeOverrideOffsetConditionParser.h"
#include "JSONFormParser.h"
#include "JSONConfigCachedFormParser.h"

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
			Parser<Data::configCachedForm_t> fparser;

			a_out.flags = static_cast<Data::NodeOverrideConditionFlags>(
				a_in.get("flags", stl::underlying(Data::NodeOverrideConditionFlags::kNone)).asUInt());

			a_out.node = a_in["node"].asString();
			
			fparser.Parse(a_in["form"], a_out.form, 1u);
			fparser.Parse(a_in["kw"], a_out.keyword, 1u);

			a_out.bipedSlot = a_in["bip"].asUInt();
			a_out.equipmentSlot = static_cast<Data::ObjectSlot>(a_in["slot"].asUInt());

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideCondition_t>::Create(
			const Data::configNodeOverrideCondition_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Data::configCachedForm_t> fparser;

			a_out["flags"] = stl::underlying(a_data.flags.value);
			a_out["node"] = *a_data.node;

			fparser.Create(a_data.form, a_out["form"]);
			fparser.Create(a_data.keyword, a_out["kw"]);

			a_out["bip"] = a_data.bipedSlot;
			a_out["slot"] = stl::underlying(a_data.equipmentSlot);
		}

		template <>
		void Parser<Data::configNodeOverrideCondition_t>::GetDefault(
			Data::configNodeOverrideCondition_t& a_out) const
		{
		}
	}
}
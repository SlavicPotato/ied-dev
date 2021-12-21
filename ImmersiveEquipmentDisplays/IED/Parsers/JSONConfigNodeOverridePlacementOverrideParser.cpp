#include "pch.h"

#include "JSONConfigNodeOverrideOffsetConditionListParser.h"
#include "JSONConfigNodeOverridePlacementOverrideParser.h"
#include "JSONConfigNodeOverridePlacementValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePlacementOverride_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverridePlacementOverride_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configNodeOverridePlacementValues_t> vparser;
			Parser<Data::configNodeOverrideConditionList_t> mlparser;

			if (!vparser.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			if (!mlparser.Parse(a_in["matches"], a_out.conditions))
			{
				return false;
			}

			a_out.overrideFlags = static_cast<Data::NodeOverridePlacementOverrideFlags>(
				a_in.get("override_flags", stl::underlying(Data::NodeOverridePlacementOverrideFlags::kNone)).asUInt());

			a_out.description = a_in.get("desc", "").asString();

			return true;
		}

		template <>
		void Parser<Data::configNodeOverridePlacementOverride_t>::Create(
			const Data::configNodeOverridePlacementOverride_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Data::configNodeOverridePlacementValues_t> vparser;
			Parser<Data::configNodeOverrideConditionList_t> mlparser;

			vparser.Create(a_data, a_out);
			mlparser.Create(a_data.conditions, a_out["matches"]);

			a_out["override_flags"] = stl::underlying(a_data.overrideFlags.value);
			a_out["desc"] = a_data.description;
		}

		template <>
		void Parser<Data::configNodeOverridePlacementOverride_t>::GetDefault(
			Data::configNodeOverridePlacementOverride_t& a_out) const
		{
		}
	}
}
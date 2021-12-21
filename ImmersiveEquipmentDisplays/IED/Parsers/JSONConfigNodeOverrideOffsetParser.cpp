#include "pch.h"

#include "JSONConfigNodeOverrideOffsetListParser.h"
#include "JSONConfigNodeOverrideOffsetConditionListParser.h"
#include "JSONConfigNodeOverrideOffsetParser.h"
#include "JSONConfigNodeOverrideValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideOffset_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideOffset_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configNodeOverrideValues_t> vparser;
			Parser<Data::configNodeOverrideConditionList_t> mlparser;
			Parser<Data::configNodeOverrideOffsetList_t> olparser;

			if (!vparser.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			if (!mlparser.Parse(a_in["matches"], a_out.conditions))
			{
				return false;
			}

			a_out.offsetFlags = static_cast<Data::NodeOverrideOffsetFlags>(
				a_in.get("offset_flags", stl::underlying(Data::NodeOverrideOffsetFlags::kNone)).asUInt());

			a_out.description = a_in.get("desc", "").asString();

			ParseFloatArray(a_in["adjust_scale"], a_out.adjustScale, 3);
			a_out.clamp();

			if (!olparser.Parse(a_in["group"], a_out.group))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideOffset_t>::Create(
			const Data::configNodeOverrideOffset_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Data::configNodeOverrideValues_t> vparser;
			Parser<Data::configNodeOverrideConditionList_t> mlparser;
			Parser<Data::configNodeOverrideOffsetList_t> olparser;

			vparser.Create(a_data, a_out);
			mlparser.Create(a_data.conditions, a_out["matches"]);

			a_out["offset_flags"] = stl::underlying(a_data.offsetFlags.value);
			a_out["desc"] = a_data.description;

			CreateFloatArray(a_data.adjustScale, 3, a_out["adjust_scale"]);
			
			if (!a_data.group.empty())
			{
				olparser.Create(a_data.group, a_out["group"]);
			}
		}

		template <>
		void Parser<Data::configNodeOverrideOffset_t>::GetDefault(
			Data::configNodeOverrideOffset_t& a_out) const
		{
		}
	}
}
#include "pch.h"

#include "JSONConfigNodeOverrideConditionListParser.h"
#include "JSONConfigNodeOverrideOffsetListParser.h"
#include "JSONConfigNodeOverrideOffsetParser.h"
#include "JSONConfigNodeOverrideValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideOffset_t>::Parse(
			const Json::Value&                a_in,
			Data::configNodeOverrideOffset_t& a_out,
			const std::uint32_t               a_version) const
		{
			Parser<Data::configNodeOverrideValues_t> vparser(m_state);

			if (!vparser.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			if (auto& matches = a_in["matches"])
			{
				Parser<Data::configNodeOverrideConditionList_t> mlparser(m_state);

				if (!mlparser.Parse(matches, a_out.conditions))
				{
					return false;
				}
			}

			a_out.offsetFlags = static_cast<Data::NodeOverrideOffsetFlags>(
				a_in.get("offset_flags", stl::underlying(Data::NodeOverrideOffsetFlags::kNone)).asUInt());

			if (auto& desc = a_in["desc"])
			{
				a_out.description = desc.asString();
			}

			ParseFloatArray(a_in["adjust_scale"], a_out.adjustScale, 3);
			a_out.clamp();

			if (auto& group = a_in["group"])
			{
				Parser<Data::configNodeOverrideOffsetList_t> olparser(m_state);

				if (!olparser.Parse(group, a_out.group))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideOffset_t>::Create(
			const Data::configNodeOverrideOffset_t& a_data,
			Json::Value&                            a_out) const
		{
			Parser<Data::configNodeOverrideValues_t> vparser(m_state);

			vparser.Create(a_data, a_out);

			if (!a_data.conditions.empty())
			{
				Parser<Data::configNodeOverrideConditionList_t> mlparser(m_state);

				mlparser.Create(a_data.conditions, a_out["matches"]);
			}

			a_out["offset_flags"] = stl::underlying(a_data.offsetFlags.value);

			if (!a_data.description.empty())
			{
				a_out["desc"] = a_data.description;
			}

			CreateFloatArray(a_data.adjustScale, 3, a_out["adjust_scale"]);

			if (!a_data.group.empty())
			{
				Parser<Data::configNodeOverrideOffsetList_t> olparser(m_state);

				olparser.Create(a_data.group, a_out["group"]);
			}
		}

	}
}
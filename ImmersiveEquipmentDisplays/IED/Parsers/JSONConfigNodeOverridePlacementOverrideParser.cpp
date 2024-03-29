#include "pch.h"

#include "JSONConfigNodeOverridePlacementOverrideParser.h"

#include "JSONConfigNodeOverrideConditionListParser.h"
#include "JSONConfigNodeOverridePlacementValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePlacementOverride_t>::Parse(
			const Json::Value&                           a_in,
			Data::configNodeOverridePlacementOverride_t& a_out,
			const std::uint32_t                          a_version) const
		{
			Parser<Data::configNodeOverridePlacementValues_t> vparser(m_state);

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

			a_out.overrideFlags = static_cast<Data::NodeOverridePlacementOverrideFlags>(
				a_in.get("override_flags", stl::underlying(Data::NodeOverridePlacementOverrideFlags::kNone)).asUInt());

			a_out.description = a_in.get("desc", "").asString();

			return true;
		}

		template <>
		void Parser<Data::configNodeOverridePlacementOverride_t>::Create(
			const Data::configNodeOverridePlacementOverride_t& a_data,
			Json::Value&                                       a_out) const
		{
			Parser<Data::configNodeOverridePlacementValues_t> vparser(m_state);

			vparser.Create(a_data, a_out);

			if (!a_data.conditions.empty())
			{
				Parser<Data::configNodeOverrideConditionList_t> mlparser(m_state);

				mlparser.Create(a_data.conditions, a_out["matches"]);
			}

			a_out["override_flags"] = stl::underlying(a_data.overrideFlags.value);
			a_out["desc"]           = a_data.description;
		}

	}
}
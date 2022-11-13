#include "pch.h"

#include "JSONConfigNodeOverridePlacementParser.h"

#include "JSONConfigNodeOverridePlacementOverrideListParser.h"
#include "JSONConfigNodeOverridePlacementValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePlacement_t>::Parse(
			const Json::Value&                   a_in,
			Data::configNodeOverridePlacement_t& a_out,
			const std::uint32_t                  a_version) const
		{
			Parser<Data::configNodeOverridePlacementValues_t> vparser(m_state);

			if (!vparser.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			if (auto& overrides = a_in["overrides"])
			{
				Parser<Data::configNodeOverridePlacementOverrideList_t> lparser(m_state);

				if (!lparser.Parse(overrides, a_out.overrides))
				{
					return false;
				}
			}

			a_out.pflags = static_cast<Data::NodeOverridePlacementFlags>(
				a_in.get("parent_flags", stl::underlying(Data::NodeOverridePlacementFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configNodeOverridePlacement_t>::Create(
			const Data::configNodeOverridePlacement_t& a_data,
			Json::Value&                               a_out) const
		{
			Parser<Data::configNodeOverridePlacementValues_t> vparser(m_state);

			vparser.Create(a_data, a_out);

			if (!a_data.overrides.empty())
			{
				Parser<Data::configNodeOverridePlacementOverrideList_t> lparser(m_state);

				lparser.Create(a_data.overrides, a_out["overrides"]);
			}

			a_out["parent_flags"] = stl::underlying(a_data.pflags.value);
		}

	}
}
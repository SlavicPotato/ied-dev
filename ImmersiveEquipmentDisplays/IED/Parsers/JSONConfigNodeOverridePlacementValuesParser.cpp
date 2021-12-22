#include "pch.h"

#include "JSONConfigNodeOverridePlacementValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePlacementValues_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverridePlacementValues_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configTransform_t> tfparser(m_state);

			a_out.targetNode = a_in.get("target", "").asString();

			a_out.flags = static_cast<Data::NodeOverridePlacementValuesFlags>(
				a_in.get("flags", stl::underlying(Data::NodeOverridePlacementValuesFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configNodeOverridePlacementValues_t>::Create(
			const Data::configNodeOverridePlacementValues_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Data::configTransform_t> tfparser(m_state);

			a_out["target"] = *a_data.targetNode;
			a_out["flags"] = stl::underlying(a_data.flags.value);
		}

		template <>
		void Parser<Data::configNodeOverridePlacementValues_t>::GetDefault(
			Data::configNodeOverridePlacementValues_t& a_out) const
		{
		}
	}
}
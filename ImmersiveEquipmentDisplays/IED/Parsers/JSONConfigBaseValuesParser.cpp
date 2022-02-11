#include "pch.h"

#include "JSONConfigBaseValuesParser.h"
#include "JSONConfigTransformParser.h"
#include "JSONNodeMapValueParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configBaseValues_t>::Parse(
			const Json::Value& a_in,
			Data::configBaseValues_t& a_out,
			const std::uint32_t a_version) const
		{
			a_out.flags = static_cast<Data::BaseFlags>(
				a_in.get("flags", stl::underlying(Data::configBaseValues_t::DEFAULT_FLAGS)).asUInt());

			Parser<Data::NodeDescriptor> nvParser(m_state);
			Parser<Data::configTransform_t> tfParser(m_state);

			if (!tfParser.Parse(a_in["xfrm"], a_out, a_version))
			{
				return false;
			}

			if (!nvParser.Parse(a_in["node"], a_out.targetNode, a_version))
			{
				return false;
			}

			a_out.targetNode.lookup_flags();

			return true;
		}

		template <>
		void Parser<Data::configBaseValues_t>::Create(
			const Data::configBaseValues_t& a_data,
			Json::Value& a_out) const
		{
			a_out["flags"] = stl::underlying(a_data.flags.value);

			Parser<Data::NodeDescriptor> nvParser(m_state);
			Parser<Data::configTransform_t> tfParser(m_state);

			tfParser.Create(a_data, a_out["xfrm"]);
			nvParser.Create(a_data.targetNode, a_out["node"]);
		}

		template <>
		void Parser<Data::configBaseValues_t>::GetDefault(Data::configBaseValues_t& a_out) const
		{}
	}  // namespace Serialization
}  // namespace IED
#include "pch.h"

#include "JSONNodeMapValueParser.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		template <>
		bool Parser<Data::NodeMap::value_type>::Parse(
			const Json::Value& a_in,
			Data::NodeMap::value_type& a_out,
			const std::uint32_t a_version) const
		{
			a_out.name = a_in["name"].asString();
			a_out.flags = static_cast<NodeDescriptorFlags>(
				a_in.get("flags", stl::underlying(NodeDescriptorFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::NodeMap::value_type>::Create(
			const Data::NodeMap::value_type& a_data,
			Json::Value& a_out) const
		{
			a_out["name"] = a_data.name.c_str();
			a_out["flags"] = stl::underlying(a_data.flags.value);
		}

		template <>
		void Parser<Data::NodeMap::value_type>::GetDefault(
			Data::NodeMap::value_type& a_out) const
		{
		}

	}
}
#pragma once

#include "Serialization/Serialization.h"

#include "IED/NodeMap.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::NodeMap::value_type>::Parse(
			const Json::Value&         a_in,
			Data::NodeMap::value_type& a_outData,
			const std::uint32_t        a_versions) const;

		template <>
		void Parser<Data::NodeMap::value_type>::Create(
			const Data::NodeMap::value_type& a_data,
			Json::Value&                     a_out) const;

	}
}
#pragma once

#include "Serialization/Serialization.h"

#include "IED/NodeMap.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::NodeMap::map_type>::Parse(
			const Json::Value&       a_in,
			Data::NodeMap::map_type& a_outData,
			bool                     a_extraOnly) const;

		template <>
		void Parser<Data::NodeMap::map_type>::Create(
			const Data::NodeMap::map_type& a_data,
			Json::Value&                   a_out,
			bool                           a_extraOnly) const;

	}  // namespace Serialization
}  // namespace IED
#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePlacementOverride_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverridePlacementOverride_t& a_outData,
			const std::uint32_t a_version) const;

		template <>
		void Parser<Data::configNodeOverridePlacementOverride_t>::Create(
			const Data::configNodeOverridePlacementOverride_t& a_data,
			Json::Value& a_out) const;

	}  // namespace Serialization
}  // namespace IED
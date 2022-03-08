#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePlacementValues_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverridePlacementValues_t& a_outData,
			const std::uint32_t a_version) const;

		template <>
		void Parser<Data::configNodeOverridePlacementValues_t>::Create(
			const Data::configNodeOverridePlacementValues_t& a_data,
			Json::Value& a_out) const;

	}
}
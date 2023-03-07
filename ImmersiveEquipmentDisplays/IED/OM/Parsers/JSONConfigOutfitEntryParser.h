#pragma once

#include "IED/OM/ConfigOutfitEntry.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::OM::configOutfitEntry_t>::Parse(
			const Json::Value&        a_in,
			Data::OM::configOutfitEntry_t& a_out,
			const std::uint32_t       a_version) const;

		template <>
		void Parser<Data::OM::configOutfitEntry_t>::Create(
			const Data::OM::configOutfitEntry_t& a_data,
			Json::Value&                    a_out) const;

	}
}
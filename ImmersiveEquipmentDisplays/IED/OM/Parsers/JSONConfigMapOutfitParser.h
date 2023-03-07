#pragma once

#include "IED/OM/ConfigStoreOutfit.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::OM::configStoreOutfit_t::map_type>::Parse(
			const Json::Value&                       a_in,
			Data::OM::configStoreOutfit_t::map_type& a_out,
			const std::uint32_t                      a_version) const;

		template <>
		void Parser<Data::OM::configStoreOutfit_t::map_type>::Create(
			const Data::OM::configStoreOutfit_t::map_type& a_data,
			Json::Value&                                   a_out) const;

	}
}
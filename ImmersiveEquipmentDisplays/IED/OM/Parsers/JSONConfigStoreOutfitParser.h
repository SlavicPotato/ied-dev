#pragma once

#include "IED/OM/ConfigStoreOutfit.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::OM::configStoreOutfit_t>::Parse(
			const Json::Value&         a_in,
			Data::OM::configStoreOutfit_t& a_out) const;

		template <>
		void Parser<Data::OM::configStoreOutfit_t>::Create(
			const Data::OM::configStoreOutfit_t& a_in,
			Json::Value&                     a_out) const;

	}
}
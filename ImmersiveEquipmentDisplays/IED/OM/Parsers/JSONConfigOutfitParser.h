#pragma once

#include "IED/OM/ConfigOutfit.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::OM::configOutfit_t>::Parse(
			const Json::Value&        a_in,
			Data::OM::configOutfit_t& a_out,
			const std::uint32_t       a_version) const;

		template <>
		void Parser<Data::OM::configOutfit_t>::Create(
			const Data::OM::configOutfit_t& a_data,
			Json::Value&                    a_out) const;

	}
}
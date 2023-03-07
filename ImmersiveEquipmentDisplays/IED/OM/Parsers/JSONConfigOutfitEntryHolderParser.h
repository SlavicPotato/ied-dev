#pragma once

#include "IED/OM/ConfigOutfitEntryHolder.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::OM::configOutfitEntryHolder_t>::Parse(
			const Json::Value&                   a_in,
			Data::OM::configOutfitEntryHolder_t& a_out) const;

		template <>
		void Parser<Data::OM::configOutfitEntryHolder_t>::Create(
			const Data::OM::configOutfitEntryHolder_t& a_data,
			Json::Value&                               a_out) const;

	}
}
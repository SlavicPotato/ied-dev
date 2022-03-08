#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configStoreSlot_t>::Parse(
			const Json::Value& a_in,
			Data::configStoreSlot_t& a_outData) const;

		template <>
		void Parser<Data::configStoreSlot_t>::Create(
			const Data::configStoreSlot_t& a_data,
			Json::Value& a_out) const;

	}  // namespace Serialization
}  // namespace IED
#pragma once

#include "IED/ConfigStore.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configStoreNodeOverride_t>::Parse(
			const Json::Value&               a_in,
			Data::configStoreNodeOverride_t& a_outData) const;

		template <>
		void Parser<Data::configStoreNodeOverride_t>::Create(
			const Data::configStoreNodeOverride_t& a_data,
			Json::Value&                           a_out) const;

	}
}
#pragma once

#include "IED/ConfigInventory.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configInventory_t>::Parse(
			const Json::Value&       a_in,
			Data::configInventory_t& a_out,
			const std::uint32_t      a_version) const;

		template <>
		void Parser<Data::configInventory_t>::Create(
			const Data::configInventory_t& a_in,
			Json::Value&                   a_out) const;

	}
}
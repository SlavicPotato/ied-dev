#pragma once

#include "IED/ConfigKeybind.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configKeybindEntryHolder_t>::Parse(
			const Json::Value&          a_in,
			Data::configKeybindEntryHolder_t& a_out) const;

		template <>
		void Parser<Data::configKeybindEntryHolder_t>::Create(
			const Data::configKeybindEntryHolder_t& a_in,
			Json::Value&                      a_out) const;

	}
}
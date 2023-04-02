#pragma once

#include "IED/ConfigKeybind.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configKeybindEntry_t>::Parse(
			const Json::Value&       a_in,
			Data::configKeybindEntry_t& a_out) const;

		template <>
		void Parser<Data::configKeybindEntry_t>::Create(
			const Data::configKeybindEntry_t& a_in,
			Json::Value&                   a_out) const;

	}
}
#pragma once

#include "IED/ConfigStore.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configSlot_t>::Parse(
			const Json::Value&  a_in,
			Data::configSlot_t& a_out,
			const std::uint32_t a_version) const;

		template <>
		void Parser<Data::configSlot_t>::Create(
			const Data::configSlot_t& a_in,
			Json::Value&              a_out) const;

	}
}
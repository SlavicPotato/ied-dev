#pragma once

#include "IED/ConfigSlotPriority.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configSlotPriority_t>::Parse(
			const Json::Value&          a_in,
			Data::configSlotPriority_t& a_out) const;

		template <>
		void Parser<Data::configSlotPriority_t>::Create(
			const Data::configSlotPriority_t& a_in,
			Json::Value&                      a_out) const;

	}
}
#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configSlotHolder_t>::Parse(
			const Json::Value& a_in,
			Data::configSlotHolder_t& a_outData) const;

		template <>
		void Parser<Data::configSlotHolder_t>::Create(
			const Data::configSlotHolder_t& a_data,
			Json::Value& a_out) const;

	}
}
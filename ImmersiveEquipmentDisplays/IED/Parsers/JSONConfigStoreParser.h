#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configStore_t>::Parse(
			const Json::Value&   a_in,
			Data::configStore_t& a_outData) const;

		template <>
		void Parser<Data::configStore_t>::Create(
			const Data::configStore_t& a_data,
			Json::Value&               a_out) const;

	}
}
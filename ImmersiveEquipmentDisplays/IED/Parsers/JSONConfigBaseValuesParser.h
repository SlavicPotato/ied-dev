#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configBaseValues_t>::Parse(
			const Json::Value&        a_in,
			Data::configBaseValues_t& a_outData,
			const std::uint32_t       a_version) const;

		template <>
		void Parser<Data::configBaseValues_t>::Create(
			const Data::configBaseValues_t& a_data,
			Json::Value&                    a_out) const;

	}
}
#pragma once

#include "IED/ConfigNodeOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideOffset_t>::Parse(
			const Json::Value&                a_in,
			Data::configNodeOverrideOffset_t& a_outData,
			const std::uint32_t               a_version) const;

		template <>
		void Parser<Data::configNodeOverrideOffset_t>::Create(
			const Data::configNodeOverrideOffset_t& a_data,
			Json::Value&                            a_out) const;

	}
}
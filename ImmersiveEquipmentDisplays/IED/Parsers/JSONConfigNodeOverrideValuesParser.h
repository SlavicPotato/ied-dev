#pragma once

#include "IED/ConfigNodeOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideTransformValues_t>::Parse(
			const Json::Value&                a_in,
			Data::configNodeOverrideTransformValues_t& a_outData,
			const std::uint32_t               a_version) const;

		template <>
		void Parser<Data::configNodeOverrideTransformValues_t>::Create(
			const Data::configNodeOverrideTransformValues_t& a_data,
			Json::Value&                            a_out) const;

	}
}
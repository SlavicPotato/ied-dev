#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideTransform_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideTransform_t& a_outData,
			const std::uint32_t a_version) const;

		template <>
		void Parser<Data::configNodeOverrideTransform_t>::Create(
			const Data::configNodeOverrideTransform_t& a_data,
			Json::Value& a_out) const;

	}
}
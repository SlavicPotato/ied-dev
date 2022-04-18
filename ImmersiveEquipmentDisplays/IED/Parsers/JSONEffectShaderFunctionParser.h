#pragma once

#include "IED/ConfigEffectShaderFunction.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configEffectShaderFunction_t>::Parse(
			const Json::Value&                  a_in,
			Data::configEffectShaderFunction_t& a_outData,
			const std::uint32_t                 a_version) const;

		template <>
		void Parser<Data::configEffectShaderFunction_t>::Create(
			const Data::configEffectShaderFunction_t& a_data,
			Json::Value&                              a_out) const;

	}
}
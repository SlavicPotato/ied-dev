#pragma once

#include "IED/ConfigOverrideEffectShader.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configEffectShaderData_t>::Parse(
			const Json::Value&              a_in,
			Data::configEffectShaderData_t& a_outData) const;

		template <>
		void Parser<Data::configEffectShaderData_t>::Create(
			const Data::configEffectShaderData_t& a_data,
			Json::Value&                          a_out) const;

	}
}
#pragma once

#include "IED/ConfigEffectShader.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configEffectShaderHolder_t>::Parse(
			const Json::Value&                a_in,
			Data::configEffectShaderHolder_t& a_outData) const;

		template <>
		void Parser<Data::configEffectShaderHolder_t>::Create(
			const Data::configEffectShaderHolder_t& a_data,
			Json::Value&                            a_out) const;

	}
}
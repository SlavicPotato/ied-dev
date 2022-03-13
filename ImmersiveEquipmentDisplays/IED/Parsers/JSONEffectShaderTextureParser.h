#pragma once

#include "IED/ConfigEffectShader.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configEffectShaderTexture_t>::Parse(
			const Json::Value&                 a_in,
			Data::configEffectShaderTexture_t& a_outData) const;

		template <>
		void Parser<Data::configEffectShaderTexture_t>::Create(
			const Data::configEffectShaderTexture_t& a_data,
			Json::Value&                             a_out) const;

	}
}
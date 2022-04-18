#pragma once

#include "IED/ConfigEffectShaderFunction.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configEffectShaderFunctionList_t>::Parse(
			const Json::Value&                      a_in,
			Data::configEffectShaderFunctionList_t& a_outData) const;

		template <>
		void Parser<Data::configEffectShaderFunctionList_t>::Create(
			const Data::configEffectShaderFunctionList_t& a_data,
			Json::Value&                                  a_out) const;

	}
}
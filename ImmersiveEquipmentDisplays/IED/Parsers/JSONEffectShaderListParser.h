#pragma once

#include "IED/ConfigOverrideEffectShader.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::effectShaderList_t>::Parse(
			const Json::Value&                a_in,
			Data::effectShaderList_t& a_outData) const;

		template <>
		void Parser<Data::effectShaderList_t>::Create(
			const Data::effectShaderList_t& a_data,
			Json::Value&                            a_out) const;

		template <>
		void Parser<Data::effectShaderList_t>::GetDefault(
			Data::effectShaderList_t& a_out) const;

	}
}
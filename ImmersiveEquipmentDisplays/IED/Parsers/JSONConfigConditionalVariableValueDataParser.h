#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConditionalVariableValueData_t>::Parse(
			const Json::Value&                          a_in,
			Data::configConditionalVariableValueData_t& a_outData) const;

		template <>
		void Parser<Data::configConditionalVariableValueData_t>::Create(
			const Data::configConditionalVariableValueData_t& a_data,
			Json::Value&                                      a_out) const;
		
		template <>
		void Parser<Data::configConditionalVariableValueData_t>::Create(
			const Data::configConditionalVariableValueData_t& a_data,
			Json::Value&                                      a_out,
			std::uint32_t                                     a_type) const;

	}
}
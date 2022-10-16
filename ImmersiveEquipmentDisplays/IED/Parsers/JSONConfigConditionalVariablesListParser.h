#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConditionalVariablesList_t>::Parse(
			const Json::Value&                 a_in,
			Data::configConditionalVariablesList_t& a_outData) const;

		template <>
		void Parser<Data::configConditionalVariablesList_t>::Create(
			const Data::configConditionalVariablesList_t& a_data,
			Json::Value&                             a_out) const;

	}
}
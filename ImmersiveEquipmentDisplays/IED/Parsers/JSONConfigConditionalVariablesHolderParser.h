#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConditionalVariablesHolder_t>::Parse(
			const Json::Value&                           a_in,
			Data::configConditionalVariablesHolder_t& a_outData) const;

		template <>
		void Parser<Data::configConditionalVariablesHolder_t>::Create(
			const Data::configConditionalVariablesHolder_t& a_data,
			Json::Value&                                       a_out) const;

	}
}
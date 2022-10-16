#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConditionalVariablesEntry_t>::Parse(
			const Json::Value&                 a_in,
			Data::configConditionalVariablesEntry_t& a_outData) const;

		template <>
		void Parser<Data::configConditionalVariablesEntry_t>::Create(
			const Data::configConditionalVariablesEntry_t& a_data,
			Json::Value&                             a_out) const;

	}
}
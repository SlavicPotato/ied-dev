#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConditionalVariableStorage.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<conditionalVariableStorage_t>::Parse(
			const Json::Value&              a_in,
			conditionalVariableStorage_t& a_outData) const;

		template <>
		void Parser<conditionalVariableStorage_t>::Create(
			const conditionalVariableStorage_t& a_data,
			Json::Value&                          a_out) const;

	}
}
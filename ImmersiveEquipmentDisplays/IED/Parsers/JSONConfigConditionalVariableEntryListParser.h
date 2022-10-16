#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigConditionalVars.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConditionalVariablesEntryList_t>::Parse(
			const Json::Value&                       a_in,
			Data::configConditionalVariablesEntryList_t& a_outData) const;

		template <>
		void Parser<Data::configConditionalVariablesEntryList_t>::Create(
			const Data::configConditionalVariablesEntryList_t& a_data,
			Json::Value&                                   a_out) const;

	}
}
#pragma once

#include "IED/ConfigVariableSourceSelector.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configVariableSourceSelector_t>::Parse(
			const Json::Value&              a_in,
			Data::configVariableSourceSelector_t& a_outData) const;

		template <>
		void Parser<Data::configVariableSourceSelector_t>::Create(
			const Data::configVariableSourceSelector_t& a_data,
			Json::Value&                          a_out) const;

	}
}
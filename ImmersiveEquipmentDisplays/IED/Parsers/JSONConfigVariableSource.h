#pragma once

#include "IED/ConfigVariableSource.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configVariableSource_t>::Parse(
			const Json::Value&            a_in,
			Data::configVariableSource_t& a_outData) const;

		template <>
		void Parser<Data::configVariableSource_t>::Create(
			const Data::configVariableSource_t& a_data,
			Json::Value&                        a_out) const;

	}
}
#pragma once

#include "IED/ConfigConvertNodes2.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConvertNodes2_t>::Parse(
			const Json::Value&           a_in,
			Data::configConvertNodes2_t& a_outData) const;

		template <>
		void Parser<Data::configConvertNodes2_t>::Create(
			const Data::configConvertNodes2_t& a_data,
			Json::Value&                       a_out) const;

	}
}

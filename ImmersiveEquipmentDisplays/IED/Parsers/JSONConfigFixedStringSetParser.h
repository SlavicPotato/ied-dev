#pragma once

#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFixedStringSet_t>::Parse(
			const Json::Value&            a_in,
			Data::configFixedStringSet_t& a_outData) const;

		template <>
		void Parser<Data::configFixedStringSet_t>::Create(
			const Data::configFixedStringSet_t& a_data,
			Json::Value&                        a_out) const;

	}
}
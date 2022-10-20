#pragma once

#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFixedStringList_t>::Parse(
			const Json::Value&            a_in,
			Data::configFixedStringList_t& a_outData) const;

		template <>
		void Parser<Data::configFixedStringList_t>::Create(
			const Data::configFixedStringList_t& a_data,
			Json::Value&                        a_out) const;

	}
}
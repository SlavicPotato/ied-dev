#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigCommon.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configRange_t>::Parse(
			const Json::Value&   a_in,
			Data::configRange_t& a_outData) const;

		template <>
		void Parser<Data::configRange_t>::Create(
			const Data::configRange_t& a_data,
			Json::Value&               a_out) const;

	}
}
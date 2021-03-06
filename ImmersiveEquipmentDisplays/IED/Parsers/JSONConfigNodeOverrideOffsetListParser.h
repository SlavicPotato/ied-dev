#pragma once

#include "IED/ConfigNodeOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideOffsetList_t>::Parse(
			const Json::Value&                    a_in,
			Data::configNodeOverrideOffsetList_t& a_outData) const;

		template <>
		void Parser<Data::configNodeOverrideOffsetList_t>::Create(
			const Data::configNodeOverrideOffsetList_t& a_data,
			Json::Value&                                a_out) const;

	}
}
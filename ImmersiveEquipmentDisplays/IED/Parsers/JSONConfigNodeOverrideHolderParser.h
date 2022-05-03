#pragma once

#include "IED/ConfigNodeOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideHolder_t>::Parse(
			const Json::Value&                a_in,
			Data::configNodeOverrideHolder_t& a_outData) const;

		template <>
		void Parser<Data::configNodeOverrideHolder_t>::Create(
			const Data::configNodeOverrideHolder_t& a_data,
			Json::Value&                            a_out) const;

	}
}
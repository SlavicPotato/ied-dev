#pragma once

#include "IED/ConfigOverrideCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFormFilterBase_t>::Parse(
			const Json::Value&            a_in,
			Data::configFormFilterBase_t& a_out) const;

		template <>
		void Parser<Data::configFormFilterBase_t>::Create(
			const Data::configFormFilterBase_t& a_in,
			Json::Value&                        a_out) const;

	}
}
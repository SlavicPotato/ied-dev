#pragma once

#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFormFilter_t>::Parse(
			const Json::Value&        a_in,
			Data::configFormFilter_t& a_out) const;

		template <>
		void Parser<Data::configFormFilter_t>::Create(
			const Data::configFormFilter_t& a_in,
			Json::Value&                    a_out) const;

	}
}
#pragma once

#include "IED/ConfigSkeletonMatch.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configSkeletonMatch_t>::Parse(
			const Json::Value&           a_in,
			Data::configSkeletonMatch_t& a_outData) const;

		template <>
		void Parser<Data::configSkeletonMatch_t>::Create(
			const Data::configSkeletonMatch_t& a_data,
			Json::Value&                       a_out) const;

	}
}
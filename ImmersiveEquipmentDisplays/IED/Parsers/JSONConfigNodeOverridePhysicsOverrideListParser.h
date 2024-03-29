#pragma once

#include "IED/ConfigNodeOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePhysicsOverrideList_t>::Parse(
			const Json::Value&                               a_in,
			Data::configNodeOverridePhysicsOverrideList_t& a_outData) const;

		template <>
		void Parser<Data::configNodeOverridePhysicsOverrideList_t>::Create(
			const Data::configNodeOverridePhysicsOverrideList_t& a_data,
			Json::Value&                                           a_out) const;

	}
}
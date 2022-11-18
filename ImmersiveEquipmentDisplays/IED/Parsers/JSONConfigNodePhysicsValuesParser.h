#pragma once

#include "IED/ConfigNodePhysicsValues.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodePhysicsValues_t>::Parse(
			const Json::Value&               a_in,
			Data::configNodePhysicsValues_t& a_outData) const;

		template <>
		void Parser<Data::configNodePhysicsValues_t>::Create(
			const Data::configNodePhysicsValues_t& a_data,
			Json::Value&                           a_out) const;

	}
}
#pragma once

#include "IED/ConfigAdditionalCMENode.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configAdditionalCMENode_t>::Parse(
			const Json::Value&           a_in,
			Data::configAdditionalCMENode_t& a_outData) const;

		template <>
		void Parser<Data::configAdditionalCMENode_t>::Create(
			const Data::configAdditionalCMENode_t& a_data,
			Json::Value&                       a_out) const;

	}
}
#pragma once

#include "IED/ConfigAdditionalCMENode.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configAdditionalCMENodeList_t>::Parse(
			const Json::Value&               a_in,
			Data::configAdditionalCMENodeList_t& a_outData) const;

		template <>
		void Parser<Data::configAdditionalCMENodeList_t>::Create(
			const Data::configAdditionalCMENodeList_t& a_data,
			Json::Value&                           a_out) const;

	}
}
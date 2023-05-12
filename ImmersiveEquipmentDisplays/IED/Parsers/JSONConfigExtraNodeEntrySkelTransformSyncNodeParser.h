#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigExtraNode.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeEntrySkelTransformSyncNode_t>::Parse(
			const Json::Value&            a_in,
			Data::configExtraNodeEntrySkelTransformSyncNode_t& a_outData) const;

		template <>
		void Parser<Data::configExtraNodeEntrySkelTransformSyncNode_t>::Create(
			const Data::configExtraNodeEntrySkelTransformSyncNode_t& a_data,
			Json::Value&                        a_out) const;

	}
}
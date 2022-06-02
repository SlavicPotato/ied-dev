#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigExtraNode.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeMap_t>::Parse(
			const Json::Value&          a_in,
			Data::configExtraNodeMap_t& a_outData) const;

		template <>
		void Parser<Data::configExtraNodeMap_t>::Create(
			const Data::configExtraNodeMap_t& a_data,
			Json::Value&                      a_out) const;

	}
}
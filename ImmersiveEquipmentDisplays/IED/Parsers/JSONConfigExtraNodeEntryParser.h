#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigExtraNode.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeEntry_t>::Parse(
			const Json::Value&       a_in,
			Data::configExtraNodeEntry_t& a_outData) const;

		template <>
		void Parser<Data::configExtraNodeEntry_t>::Create(
			const Data::configExtraNodeEntry_t& a_data,
			Json::Value&                   a_out) const;

	}
}
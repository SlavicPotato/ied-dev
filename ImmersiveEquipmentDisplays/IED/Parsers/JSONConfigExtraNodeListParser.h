#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigExtraNode.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeList_t>::Parse(
			const Json::Value&          a_in,
			Data::configExtraNodeList_t& a_outData) const;

		template <>
		void Parser<Data::configExtraNodeList_t>::Create(
			const Data::configExtraNodeList_t& a_data,
			Json::Value&                      a_out) const;

	}
}
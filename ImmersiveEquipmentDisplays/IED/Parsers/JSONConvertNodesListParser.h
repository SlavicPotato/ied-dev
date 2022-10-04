#pragma once

#include "IED/ConfigConvertNodes.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConvertNodesList_t>::Parse(
			const Json::Value&              a_in,
			Data::configConvertNodesList_t& a_outData) const;

		template <>
		void Parser<Data::configConvertNodesList_t>::Create(
			const Data::configConvertNodesList_t& a_data,
			Json::Value&                          a_out) const;

	}
}
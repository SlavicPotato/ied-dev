#pragma once

#include "IED/ConfigExtraLight.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraLight_t>::Parse(
			const Json::Value&        a_in,
			Data::configExtraLight_t& a_outData) const;

		template <>
		void Parser<Data::configExtraLight_t>::Create(
			const Data::configExtraLight_t& a_data,
			Json::Value&                    a_out) const;

	}
}
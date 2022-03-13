#pragma once

#include "IED/ConfigColor.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configColorRGBA_t>::Parse(
			const Json::Value&       a_in,
			Data::configColorRGBA_t& a_outData,
			float                    a_defaultAlpha) const;

		template <>
		void Parser<Data::configColorRGBA_t>::Create(
			const Data::configColorRGBA_t& a_data,
			Json::Value&                   a_out) const;

	}
}
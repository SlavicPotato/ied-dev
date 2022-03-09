#pragma once

#include "IED/ConfigOverrideColor.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configColorRGB_t>::Parse(
			const Json::Value&      a_in,
			Data::configColorRGB_t& a_outData) const;

		template <>
		void Parser<Data::configColorRGB_t>::Create(
			const Data::configColorRGB_t& a_data,
			Json::Value&                  a_out) const;

	}
}
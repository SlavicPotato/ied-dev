#pragma once

#include "Serialization/Serialization.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<fontGlyphData_t>::Parse(
			const Json::Value& a_in,
			fontGlyphData_t&   a_outData) const;

		template <>
		void Parser<fontGlyphData_t>::Create(
			const fontGlyphData_t& a_data,
			Json::Value&           a_out) const;

	}
}
#pragma once

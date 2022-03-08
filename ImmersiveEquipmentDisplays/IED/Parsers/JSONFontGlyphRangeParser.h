#pragma once

#include "Serialization/Serialization.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<fontGlyphRange_t>::Parse(
			const Json::Value& a_in,
			fontGlyphRange_t&  a_outData) const;

		template <>
		void Parser<fontGlyphRange_t>::Create(
			const fontGlyphRange_t& a_data,
			Json::Value&            a_out) const;

	}
}
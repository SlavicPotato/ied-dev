#pragma once

#include "Serialization/Serialization.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<FontGlyphData>::Parse(
			const Json::Value& a_in,
			FontGlyphData&   a_outData) const;

		template <>
		void Parser<FontGlyphData>::Create(
			const FontGlyphData& a_data,
			Json::Value&           a_out) const;

	}
}
#pragma once

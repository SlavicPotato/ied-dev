#pragma once

#include "Serialization/Serialization.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<FontInfoMap>::Parse(
			const Json::Value& a_in,
			FontInfoMap&     a_outData) const;

		template <>
		void Parser<FontInfoMap>::Create(
			const FontInfoMap& a_data,
			Json::Value&         a_out) const;

	}
}
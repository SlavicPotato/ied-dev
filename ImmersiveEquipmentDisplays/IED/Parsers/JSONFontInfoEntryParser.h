#pragma once

#include "Serialization/Serialization.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<FontInfoEntry>::Parse(
			const Json::Value& a_in,
			FontInfoEntry&   a_outData,
			std::uint32_t      a_version) const;

		template <>
		void Parser<FontInfoEntry>::Create(
			const FontInfoEntry& a_data,
			Json::Value&           a_out) const;

	}
}
#pragma once

#include "Serialization/Serialization.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<fontInfoEntry_t>::Parse(
			const Json::Value& a_in,
			fontInfoEntry_t& a_outData,
			std::uint32_t a_version) const;

		template <>
		void Parser<fontInfoEntry_t>::Create(
			const fontInfoEntry_t& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<fontInfoEntry_t>::GetDefault(
			fontInfoEntry_t& a_out) const;

	}
}
#pragma once

#include "Serialization/Serialization.h"

#include "Fonts/FontInfo.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<fontInfoMap_t>::Parse(
			const Json::Value& a_in,
			fontInfoMap_t& a_outData) const;

		template <>
		void Parser<fontInfoMap_t>::Create(
			const fontInfoMap_t& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<fontInfoMap_t>::GetDefault(
			fontInfoMap_t& a_out) const;

	}
}
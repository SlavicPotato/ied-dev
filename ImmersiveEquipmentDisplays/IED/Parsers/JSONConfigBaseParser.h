#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configBase_t>::Parse(
			const Json::Value& a_in,
			Data::configBase_t& a_outData,
			const std::uint32_t a_version) const;

		template <>
		void Parser<Data::configBase_t>::Create(
			const Data::configBase_t& a_data,
			Json::Value& a_out) const;

	}
}
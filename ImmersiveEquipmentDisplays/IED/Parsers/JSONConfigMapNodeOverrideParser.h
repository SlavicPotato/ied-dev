#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configMapNodeOverrides_t>::Parse(
			const Json::Value& a_in,
			Data::configMapNodeOverrides_t& a_outData) const;

		template <>
		void Parser<Data::configMapNodeOverrides_t>::Create(
			const Data::configMapNodeOverrides_t& a_data,
			Json::Value& a_out) const;

	}
}
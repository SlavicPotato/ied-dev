#pragma once

#include "IED/ConfigOverrideCustom.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configMapCustom_t>::Parse(
			const Json::Value&       a_in,
			Data::configMapCustom_t& a_out) const;

		template <>
		void Parser<Data::configMapCustom_t>::Create(
			const Data::configMapCustom_t& a_in,
			Json::Value&                   a_out) const;

	}
}
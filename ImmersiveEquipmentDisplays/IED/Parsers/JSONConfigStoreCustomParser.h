#pragma once

#include "IED/ConfigOverrideCustom.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configStoreCustom_t>::Parse(
			const Json::Value& a_in,
			Data::configStoreCustom_t& a_out) const;

		template <>
		void Parser<Data::configStoreCustom_t>::Create(
			const Data::configStoreCustom_t& a_in,
			Json::Value& a_out) const;

	}
}
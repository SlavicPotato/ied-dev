#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configMapSlot_t>::Parse(
			const Json::Value& a_in,
			Data::configMapSlot_t& a_outData) const;

		template <>
		void Parser<Data::configMapSlot_t>::Create(
			const Data::configMapSlot_t& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::configMapSlot_t>::GetDefault(
			Data::configMapSlot_t& a_out) const;

	}  // namespace Serialization
}  // namespace IED
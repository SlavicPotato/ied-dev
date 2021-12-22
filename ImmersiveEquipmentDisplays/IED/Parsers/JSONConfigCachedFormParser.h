#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigOverrideCommon.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configCachedForm_t>::Parse(
			const Json::Value& a_in,
			Data::configCachedForm_t& a_outData) const;

		template <>
		void Parser<Data::configCachedForm_t>::Create(
			const Data::configCachedForm_t& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::configCachedForm_t>::GetDefault(Data::configCachedForm_t& a_out) const;

	}  // namespace Serialization
}  // namespace IED
#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideConditionList_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideConditionList_t& a_outData) const;

		template <>
		void Parser<Data::configNodeOverrideConditionList_t>::Create(
			const Data::configNodeOverrideConditionList_t& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::configNodeOverrideConditionList_t>::GetDefault(
			Data::configNodeOverrideConditionList_t& a_out) const;

	}  // namespace Serialization
}  // namespace IED
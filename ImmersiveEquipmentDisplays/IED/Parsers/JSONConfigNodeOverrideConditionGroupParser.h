#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideConditionGroup_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideConditionGroup_t& a_outData) const;

		template <>
		void Parser<Data::configNodeOverrideConditionGroup_t>::Create(
			const Data::configNodeOverrideConditionGroup_t& a_data,
			Json::Value& a_out) const;

	}
}
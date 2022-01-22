#pragma once

#include "IED/ConfigOverride.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverrideConditionGroup_t>::Parse(
			const Json::Value& a_in,
			Data::equipmentOverrideConditionGroup_t& a_outData) const;

		template <>
		void Parser<Data::equipmentOverrideConditionGroup_t>::Create(
			const Data::equipmentOverrideConditionGroup_t& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::equipmentOverrideConditionGroup_t>::GetDefault(
			Data::equipmentOverrideConditionGroup_t& a_out) const;

	}
}
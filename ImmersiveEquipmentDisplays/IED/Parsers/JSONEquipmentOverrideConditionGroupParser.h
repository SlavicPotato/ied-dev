#pragma once

#include "IED/ConfigStore.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverrideConditionGroup_t>::Parse(
			const Json::Value&                       a_in,
			Data::equipmentOverrideConditionGroup_t& a_outData) const;

		template <>
		void Parser<Data::equipmentOverrideConditionGroup_t>::Create(
			const Data::equipmentOverrideConditionGroup_t& a_data,
			Json::Value&                                   a_out) const;

	}
}
#pragma once

#include "IED/ConfigEquipment.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverrideCondition_t>::Parse(
			const Json::Value&                  a_in,
			Data::equipmentOverrideCondition_t& a_outData,
			const std::uint32_t                 a_version) const;

		template <>
		void Parser<Data::equipmentOverrideCondition_t>::Create(
			const Data::equipmentOverrideCondition_t& a_data,
			Json::Value&                              a_out) const;

	}
}
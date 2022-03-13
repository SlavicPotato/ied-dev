#pragma once

#include "IED/ConfigStore.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::equipmentOverrideList_t>::Parse(
			const Json::Value&             a_in,
			Data::equipmentOverrideList_t& a_outData) const;

		template <>
		void Parser<Data::equipmentOverrideList_t>::Create(
			const Data::equipmentOverrideList_t& a_data,
			Json::Value&                         a_out) const;

	}
}
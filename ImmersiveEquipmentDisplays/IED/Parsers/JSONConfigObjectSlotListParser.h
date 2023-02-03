#pragma once

#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configObjectSlotList_t>::Parse(
			const Json::Value&            a_in,
			Data::configObjectSlotList_t& a_out) const;

		template <>
		void Parser<Data::configObjectSlotList_t>::Create(
			const Data::configObjectSlotList_t& a_in,
			Json::Value&                        a_out) const;

	}
}
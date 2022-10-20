#pragma once

#include "IED/ConfigLastEquipped.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configLastEquipped_t>::Parse(
			const Json::Value&          a_in,
			Data::configLastEquipped_t& a_out) const;

		template <>
		void Parser<Data::configLastEquipped_t>::Create(
			const Data::configLastEquipped_t& a_in,
			Json::Value&                      a_out) const;

	}
}
#pragma once

#include "IED/ConfigCustom.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configCustomHolder_t>::Parse(
			const Json::Value&          a_in,
			Data::configCustomHolder_t& a_out) const;

		template <>
		void Parser<Data::configCustomHolder_t>::Create(
			const Data::configCustomHolder_t& a_in,
			Json::Value&                      a_out) const;

	}
}
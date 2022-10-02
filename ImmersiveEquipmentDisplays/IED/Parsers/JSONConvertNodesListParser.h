#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<std::list<std::int32_t>>::Parse(
			const Json::Value&                  a_in,
			std::list<std::int32_t>& a_outData) const;

		template <>
		void Parser<std::list<std::int32_t>>::Create(
			const std::list<std::int32_t>& a_data,
			Json::Value&                              a_out) const;

	}
}
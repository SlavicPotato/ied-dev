#pragma once

#include "IED/KeyToggleStateEntryHolder.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<KB::KeyToggleStateEntryHolder>::Parse(
			const Json::Value&       a_in,
			KB::KeyToggleStateEntryHolder& a_out) const;

		template <>
		void Parser<KB::KeyToggleStateEntryHolder>::Create(
			const KB::KeyToggleStateEntryHolder& a_in,
			Json::Value&                   a_out) const;

	}
}
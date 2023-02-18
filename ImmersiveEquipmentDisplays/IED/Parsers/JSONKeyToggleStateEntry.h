#pragma once

#include "IED/KeyToggleStateEntry.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<KB::KeyToggleStateEntry>::Parse(
			const Json::Value&       a_in,
			KB::KeyToggleStateEntry& a_out) const;

		template <>
		void Parser<KB::KeyToggleStateEntry>::Create(
			const KB::KeyToggleStateEntry& a_in,
			Json::Value&                   a_out) const;

	}
}
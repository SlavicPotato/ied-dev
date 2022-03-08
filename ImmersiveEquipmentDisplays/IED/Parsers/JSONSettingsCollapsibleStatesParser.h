#pragma once

#include "Serialization/Serialization.h"

#include "IED/UI/UIData.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<UI::UIData::UICollapsibleStates>::Parse(
			const Json::Value&               a_in,
			UI::UIData::UICollapsibleStates& a_outData) const;

		template <>
		void Parser<UI::UIData::UICollapsibleStates>::Create(
			const UI::UIData::UICollapsibleStates& a_data,
			Json::Value&                           a_out) const;

	}  // namespace Serialization
}  // namespace IED
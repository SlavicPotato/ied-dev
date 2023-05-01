#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<ImVec2>::Parse(
			const Json::Value& a_in,
			ImVec2&            a_outData) const;

		template <>
		void Parser<ImVec2>::Create(
			const ImVec2& a_data,
			Json::Value&      a_out) const;

	}
}
#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<ImGuiStyle>::Parse(
			const Json::Value& a_in,
			ImGuiStyle&        a_outData) const;

		template <>
		void Parser<ImGuiStyle>::Create(
			const ImGuiStyle& a_data,
			Json::Value&     a_out) const;

	}
}
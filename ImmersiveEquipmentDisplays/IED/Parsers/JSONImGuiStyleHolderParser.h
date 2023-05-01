#pragma once

#include "Serialization/Serialization.h"

#include "IED/ImGuiStyleHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::ImGuiStyleHolder>::Parse(
			const Json::Value&      a_in,
			Data::ImGuiStyleHolder& a_outData) const;

		template <>
		void Parser<Data::ImGuiStyleHolder>::Create(
			const Data::ImGuiStyleHolder& a_data,
			Json::Value&                  a_out) const;

	}
}
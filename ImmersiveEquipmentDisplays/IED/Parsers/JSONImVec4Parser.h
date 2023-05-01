#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<ImVec4>::Parse(
			const Json::Value& a_in,
			ImVec4&            a_outData) const;

		template <>
		void Parser<ImVec4>::Create(
			const ImVec4& a_data,
			Json::Value&  a_out) const;

	}
}
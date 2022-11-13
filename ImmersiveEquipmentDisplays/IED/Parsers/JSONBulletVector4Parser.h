#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<btVector4>::Parse(
			const Json::Value& a_in,
			btVector4&         a_outData) const;

		template <>
		void Parser<btVector4>::Create(
			const btVector4& a_data,
			Json::Value&     a_out) const;

	}
}
#pragma once

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<btVector3>::Parse(
			const Json::Value& a_in,
			btVector3&         a_outData) const;

		template <>
		void Parser<btVector3>::Create(
			const btVector3& a_data,
			Json::Value&        a_out) const;

	}
}
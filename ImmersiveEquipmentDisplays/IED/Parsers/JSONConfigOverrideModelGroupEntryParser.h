#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigOverrideModelGroup.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configModelGroupEntry_t>::Parse(
			const Json::Value& a_in,
			Data::configModelGroupEntry_t& a_outData) const;

		template <>
		void Parser<Data::configModelGroupEntry_t>::Create(
			const Data::configModelGroupEntry_t& a_data,
			Json::Value& a_out) const;

	}
}
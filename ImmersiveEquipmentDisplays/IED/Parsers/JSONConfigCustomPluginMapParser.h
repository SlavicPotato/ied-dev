#pragma once

#include "IED/ConfigOverrideCustom.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configCustomPluginMap_t>::Parse(
			const Json::Value& a_in,
			Data::configCustomPluginMap_t& a_out) const;

		template <>
		void Parser<Data::configCustomPluginMap_t>::Create(
			const Data::configCustomPluginMap_t& a_in,
			Json::Value& a_out) const;

	}
}
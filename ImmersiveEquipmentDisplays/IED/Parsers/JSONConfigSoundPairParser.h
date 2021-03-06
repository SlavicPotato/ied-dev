#pragma once

#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::ConfigSound<Game::FormID>::soundPair_t>::Parse(
			const Json::Value&                            a_in,
			Data::ConfigSound<Game::FormID>::soundPair_t& a_outData) const;

		template <>
		void Parser<Data::ConfigSound<Game::FormID>::soundPair_t>::Create(
			const Data::ConfigSound<Game::FormID>::soundPair_t& a_data,
			Json::Value&                                        a_out) const;

	}
}
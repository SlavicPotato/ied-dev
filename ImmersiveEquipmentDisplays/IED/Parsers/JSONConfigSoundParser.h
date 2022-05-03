#pragma once

#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::ConfigSound<Game::FormID>>::Parse(
			const Json::Value&               a_in,
			Data::ConfigSound<Game::FormID>& a_outData) const;

		template <>
		void Parser<Data::ConfigSound<Game::FormID>>::Create(
			const Data::ConfigSound<Game::FormID>& a_data,
			Json::Value&                           a_out) const;

	}
}
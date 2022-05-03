#pragma once

#include "IED/SettingHolder.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::ConfigKeyPair>::Parse(
			const Json::Value&   a_in,
			Data::ConfigKeyPair& a_outData) const;

		template <>
		void Parser<Data::ConfigKeyPair>::Create(
			const Data::ConfigKeyPair& a_data,
			Json::Value&               a_out) const;

	}
}
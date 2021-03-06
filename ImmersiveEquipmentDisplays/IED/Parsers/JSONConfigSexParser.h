#pragma once

#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::ConfigSex>::Parse(
			const Json::Value&  a_in,
			Data::ConfigSex&    a_outData,
			const std::uint32_t a_version) const;

		template <>
		void Parser<Data::ConfigSex>::Create(
			const Data::ConfigSex& a_data,
			Json::Value&           a_out) const;

	}
}
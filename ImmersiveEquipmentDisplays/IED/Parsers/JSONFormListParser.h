#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigOverrideCommon.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFormList_t>::Parse(
			const Json::Value&      a_in,
			Data::configFormList_t& a_outData,
			const std::uint32_t     a_version) const;

		template <>
		void Parser<Data::configFormList_t>::Create(
			const Data::configFormList_t& a_data,
			Json::Value&                  a_out) const;

	}
}
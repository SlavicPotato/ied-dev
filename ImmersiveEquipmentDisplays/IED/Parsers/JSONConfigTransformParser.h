#pragma once

#include "IED/ConfigTransform.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configTransform_t>::Parse(
			const Json::Value&       a_in,
			Data::configTransform_t& a_outData,
			const std::uint32_t      a_version) const;

		template <>
		void Parser<Data::configTransform_t>::Create(
			const Data::configTransform_t& a_data,
			Json::Value&                   a_out) const;

	}
}
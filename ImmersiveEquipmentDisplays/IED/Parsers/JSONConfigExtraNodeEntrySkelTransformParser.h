#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigExtraNode.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeEntrySkelTransform_t>::Parse(
			const Json::Value&                                 a_in,
			Data::configExtraNodeEntrySkelTransform_t& a_outData) const;

		template <>
		void Parser<Data::configExtraNodeEntrySkelTransform_t>::Create(
			const Data::configExtraNodeEntrySkelTransform_t& a_data,
			Json::Value&                                             a_out) const;

	}
}
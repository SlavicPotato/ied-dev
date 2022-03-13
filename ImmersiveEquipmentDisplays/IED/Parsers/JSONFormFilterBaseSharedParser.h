#pragma once

#include "IED/ConfigCommon.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<std::shared_ptr<Data::configFormFilterBase_t>>::Parse(
			const Json::Value&                             a_in,
			std::shared_ptr<Data::configFormFilterBase_t>& a_out) const;

		template <>
		void Parser<std::shared_ptr<Data::configFormFilterBase_t>>::Create(
			const std::shared_ptr<Data::configFormFilterBase_t>& a_in,
			Json::Value&                                         a_out) const;

	}
}
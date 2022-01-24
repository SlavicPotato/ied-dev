#pragma once

#include "Serialization/Serialization.h"

#include "IED/ConfigOverrideModelGroup.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configModelGroup_t>::Parse(
			const Json::Value& a_in,
			Data::configModelGroup_t& a_outData) const;

		template <>
		void Parser<Data::configModelGroup_t>::Create(
			const Data::configModelGroup_t& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::configModelGroup_t>::GetDefault(
			Data::configModelGroup_t& a_out) const;

	}
}
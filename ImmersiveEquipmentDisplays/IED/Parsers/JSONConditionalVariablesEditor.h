#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::ConditionalVariablesEditor>::Parse(
			const Json::Value&                               a_in,
			Data::SettingHolder::ConditionalVariablesEditor& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::ConditionalVariablesEditor>::Create(
			const Data::SettingHolder::ConditionalVariablesEditor& a_data,
			Json::Value&                                           a_out) const;

	}
}
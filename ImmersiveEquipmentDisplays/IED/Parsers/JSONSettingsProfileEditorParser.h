#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::ProfileEditor>::Parse(
			const Json::Value&                  a_in,
			Data::SettingHolder::ProfileEditor& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::ProfileEditor>::Create(
			const Data::SettingHolder::ProfileEditor& a_data,
			Json::Value&                              a_out) const;

	}
}
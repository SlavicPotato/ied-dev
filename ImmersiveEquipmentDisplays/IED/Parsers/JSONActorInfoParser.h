#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::ActorInfo>::Parse(
			const Json::Value&              a_in,
			Data::SettingHolder::ActorInfo& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::ActorInfo>::Create(
			const Data::SettingHolder::ActorInfo& a_data,
			Json::Value&                          a_out) const;

	}
}
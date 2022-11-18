#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::I3DI>::Parse(
			const Json::Value&         a_in,
			Data::SettingHolder::I3DI& a_out) const;

		template <>
		void Parser<Data::SettingHolder::I3DI>::Create(
			const Data::SettingHolder::I3DI& a_data,
			Json::Value&                     a_out) const;

	}
}
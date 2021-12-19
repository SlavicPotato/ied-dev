#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::UserInterface>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::UserInterface& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::UserInterface>::Create(
			const Data::SettingHolder::UserInterface& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::SettingHolder::UserInterface>::GetDefault(
			Data::SettingHolder::UserInterface& a_out) const;

	}  // namespace Serialization
}  // namespace IED
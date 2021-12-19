#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::Settings>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::Settings& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::Settings>::Create(
			const Data::SettingHolder::Settings& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::SettingHolder::Settings>::GetDefault(
			Data::SettingHolder::Settings& a_out) const;

	}  // namespace Serialization
}  // namespace IED
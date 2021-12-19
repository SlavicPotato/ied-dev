#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::Controls>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::Controls& a_outData,
			const std::uint32_t a_version) const;

		template <>
		void Parser<Data::SettingHolder::Controls>::Create(
			const Data::SettingHolder::Controls& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::SettingHolder::Controls>::GetDefault(
			Data::SettingHolder::Controls& a_out) const;

	}  // namespace Serialization
}  // namespace IED
#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::ImportExport>::Parse(
			const Json::Value&                 a_in,
			Data::SettingHolder::ImportExport& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::ImportExport>::Create(
			const Data::SettingHolder::ImportExport& a_data,
			Json::Value&                             a_out) const;

	}  // namespace Serialization
}  // namespace IED
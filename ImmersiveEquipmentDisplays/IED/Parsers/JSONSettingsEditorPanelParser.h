#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::EditorPanel>::Parse(
			const Json::Value&                a_in,
			Data::SettingHolder::EditorPanel& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::EditorPanel>::Create(
			const Data::SettingHolder::EditorPanel& a_data,
			Json::Value&                            a_out) const;

	}  // namespace Serialization
}  // namespace IED
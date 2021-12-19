#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::EditorPanelRaceSettings>::Parse(
			const Json::Value& a_in,
			Data::SettingHolder::EditorPanelRaceSettings& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::EditorPanelRaceSettings>::Create(
			const Data::SettingHolder::EditorPanelRaceSettings& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Data::SettingHolder::EditorPanelRaceSettings>::GetDefault(
			Data::SettingHolder::EditorPanelRaceSettings& a_out) const;

	}  // namespace Serialization
}  // namespace IED
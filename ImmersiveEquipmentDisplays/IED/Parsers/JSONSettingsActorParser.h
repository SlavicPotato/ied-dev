#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::EditorPanelActorSettings>::Parse(
			const Json::Value&                             a_in,
			Data::SettingHolder::EditorPanelActorSettings& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::EditorPanelActorSettings>::Create(
			const Data::SettingHolder::EditorPanelActorSettings& a_data,
			Json::Value&                                         a_out) const;

	}
}
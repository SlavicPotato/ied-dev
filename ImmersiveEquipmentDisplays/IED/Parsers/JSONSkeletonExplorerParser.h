#pragma once

#include "Serialization/Serialization.h"

#include "IED/SettingHolder.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::SettingHolder::SkeletonExplorer>::Parse(
			const Json::Value&                     a_in,
			Data::SettingHolder::SkeletonExplorer& a_outData) const;

		template <>
		void Parser<Data::SettingHolder::SkeletonExplorer>::Create(
			const Data::SettingHolder::SkeletonExplorer& a_data,
			Json::Value&                                 a_out) const;

	}
}
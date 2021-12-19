#pragma once

#include "Serialization/Serialization.h"

#include "Localization/StringTable.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Localization::StringTable::data_storage_type>::Parse(
			const Json::Value& a_in,
			Localization::StringTable::data_storage_type& a_outData) const;

		template <>
		void Parser<Localization::StringTable::data_storage_type>::Create(
			const Localization::StringTable::data_storage_type& a_data,
			Json::Value& a_out) const;

		template <>
		void Parser<Localization::StringTable::data_storage_type>::GetDefault(
			Localization::StringTable::data_storage_type& a_out) const;

	}
}
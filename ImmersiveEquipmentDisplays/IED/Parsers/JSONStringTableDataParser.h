#pragma once

#include "Serialization/Serialization.h"

#include "Localization/StringTable.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Localization::StringTable::table_data>::Parse(
			const Json::Value&                     a_in,
			Localization::StringTable::table_data& a_outData) const;

		template <>
		void Parser<Localization::StringTable::table_data>::Create(
			const Localization::StringTable::table_data& a_data,
			Json::Value&                                 a_out) const;

	}
}
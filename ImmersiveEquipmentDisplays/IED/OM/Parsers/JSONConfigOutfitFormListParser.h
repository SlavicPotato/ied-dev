#pragma once

#include "IED/OM/ConfigOutfitForm.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::OM::configOutfitFormList_t>::Parse(
			const Json::Value&               a_in,
			Data::OM::configOutfitFormList_t& a_out) const;

		template <>
		void Parser<Data::OM::configOutfitFormList_t>::Create(
			const Data::OM::configOutfitFormList_t& a_data,
			Json::Value&                           a_out) const;

	}
}
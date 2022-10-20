#pragma once

#include "IED/ConfigBipedObjectList.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configBipedObjectList_t>::Parse(
			const Json::Value&             a_in,
			Data::configBipedObjectList_t& a_out) const;

		template <>
		void Parser<Data::configBipedObjectList_t>::Create(
			const Data::configBipedObjectList_t& a_in,
			Json::Value&                         a_out) const;

	}
}
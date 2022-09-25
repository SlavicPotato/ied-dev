#pragma once

#include "IED/ConfigNodeMonitor.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeMonitorEntryList_t>::Parse(
			const Json::Value&              a_in,
			Data::configNodeMonitorEntryList_t& a_outData) const;

		template <>
		void Parser<Data::configNodeMonitorEntryList_t>::Create(
			const Data::configNodeMonitorEntryList_t& a_data,
			Json::Value&                          a_out) const;

	}
}
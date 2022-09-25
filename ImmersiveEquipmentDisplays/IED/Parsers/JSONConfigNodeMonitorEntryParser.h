#pragma once

#include "IED/ConfigNodeMonitor.h"

#include "Serialization/Serialization.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeMonitorEntry_t>::Parse(
			const Json::Value&   a_in,
			Data::configNodeMonitorEntry_t& a_outData) const;

		template <>
		void Parser<Data::configNodeMonitorEntry_t>::Create(
			const Data::configNodeMonitorEntry_t& a_data,
			Json::Value&               a_out) const;

	}
}
#include "pch.h"

#include "JSONConfigNodeMonitorEntryListParser.h"

#include "JSONConfigNodeMonitorEntryParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeMonitorEntryList_t>::Parse(
			const Json::Value&                  a_in,
			Data::configNodeMonitorEntryList_t& a_out) const
		{
			Parser<Data::configNodeMonitorEntry_t> parser(m_state);

			for (auto& e : a_in)
			{
				if (!parser.Parse(e, a_out.emplace_back()))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeMonitorEntryList_t>::Create(
			const Data::configNodeMonitorEntryList_t& a_data,
			Json::Value&                              a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}
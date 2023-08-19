#include "pch.h"

#include "JSONConfigConvertNodes2EntryListParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConvertNodes2EntryList_t>::Parse(
			const Json::Value&                a_in,
			Data::configConvertNodes2EntryList_t& a_out) const
		{
			Parser<Data::configConvertNodes2Entry_t> parser(m_state);

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
		void Parser<Data::configConvertNodes2EntryList_t>::Create(
			const Data::configConvertNodes2EntryList_t& a_data,
			Json::Value&                            a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}
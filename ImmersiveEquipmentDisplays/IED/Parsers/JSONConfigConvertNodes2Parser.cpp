#include "pch.h"

#include "JSONConfigConvertNodes2Parser.h"

#include "JSONConfigConvertNodes2EntryParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConvertNodes2_t>::Parse(
			const Json::Value&           a_in,
			Data::configConvertNodes2_t& a_out) const
		{
			Parser<Data::configConvertNodes2Entry_t> parser(m_state);
			Parser<Data::configSkeletonMatch_t>      smparser(m_state);

			if (!smparser.Parse(a_in["match"], a_out.match))
			{
				throw parser_exception("bad match");
			}

			if (auto& entries = a_in["entries"])
			{
				for (auto it = entries.begin(); it != entries.end(); ++it)
				{
					auto r = a_out.entries.try_emplace(it.key().asString());

					if (!parser.Parse(*it, r.first->second))
					{
						return false;
					}
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configConvertNodes2_t>::Create(
			const Data::configConvertNodes2_t& a_data,
			Json::Value&                       a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}
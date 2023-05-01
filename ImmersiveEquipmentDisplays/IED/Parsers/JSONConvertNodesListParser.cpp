#include "pch.h"

#include "JSONConvertNodesListParser.h"

#include "JSONConfigSkeletonMatchParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConvertNodesList_t>::Parse(
			const Json::Value&              a_in,
			Data::configConvertNodesList_t& a_out) const
		{
			Parser<Data::configSkeletonMatch_t> parser(m_state);

			for (auto& e : a_in)
			{
				auto& v = a_out.emplace_back();

				if (!parser.Parse(e["match"], v.match))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configConvertNodesList_t>::Create(
			const Data::configConvertNodesList_t& a_data,
			Json::Value&                          a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}
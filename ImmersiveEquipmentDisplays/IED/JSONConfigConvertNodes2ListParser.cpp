#include "pch.h"

#include "JSONConfigConvertNodes2ListParser.h"

#include "JSONConfigConvertNodes2ListParser.h

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConvertNodesList2_t>::Parse(
			const Json::Value&               a_in,
			Data::configConvertNodesList2_t& a_out) const
		{
			Parser<Data::configConvertNodes2_t> parser(m_state);

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
		void Parser<Data::configConvertNodesList2_t>::Create(
			const Data::configConvertNodesList2_t& a_data,
			Json::Value&                           a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}
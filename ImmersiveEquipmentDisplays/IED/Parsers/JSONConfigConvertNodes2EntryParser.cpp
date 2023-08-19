#include "pch.h"

#include "JSONConfigConvertNodes2EntryParser.h"

#include "JSONConfigTransformParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configConvertNodes2Entry_t>::Parse(
			const Json::Value&                a_in,
			Data::configConvertNodes2Entry_t& a_out) const
		{
			Parser<Data::configTransform_t>     xfrmparser(m_state);

			if (!xfrmparser.Parse(a_in["xfrm_mov"], a_out.xfrmMOV, std::uint32_t(0)))
			{
				throw parser_exception("bad xfrm");
			}

			if (!xfrmparser.Parse(a_in["xfrm_node"], a_out.xfrmNode, std::uint32_t(0)))
			{
				throw parser_exception("bad xfrm");
			}

			return true;
		}

		template <>
		void Parser<Data::configConvertNodes2Entry_t>::Create(
			const Data::configConvertNodes2Entry_t& a_data,
			Json::Value&                            a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}
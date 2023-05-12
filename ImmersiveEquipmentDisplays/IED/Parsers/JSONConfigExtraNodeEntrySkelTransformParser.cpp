#include "pch.h"

#include "JSONConfigExtraNodeEntrySkelTransformParser.h"

#include "JSONConfigExtraNodeEntrySkelTransformSyncNodeParser.h"
#include "JSONConfigTransformParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeEntrySkelTransform_t>::Parse(
			const Json::Value&                         a_in,
			Data::configExtraNodeEntrySkelTransform_t& a_out) const
		{
			Parser<Data::configExtraNodeEntrySkelTransformSyncNode_t> syncparser(m_state);
			Parser<Data::configTransform_t>                           xfrmparser(m_state);

			if (!xfrmparser.Parse(a_in, a_out.xfrm, std::uint32_t(0)))
			{
				throw parser_exception("bad xfrm");
			}

			for (auto& e : a_in["sync"])
			{
				if (!syncparser.Parse(e, a_out.syncNodes.emplace_back()))
				{
					throw parser_exception("bad sync");
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configExtraNodeEntrySkelTransform_t>::Create(
			const Data::configExtraNodeEntrySkelTransform_t& a_data,
			Json::Value&                                     a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}
	}
}
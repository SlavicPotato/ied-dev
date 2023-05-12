#include "pch.h"

#include "JSONConfigExtraNodeEntrySkelTransformSyncNodeParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configExtraNodeEntrySkelTransformSyncNode_t>::Parse(
			const Json::Value&                                 a_in,
			Data::configExtraNodeEntrySkelTransformSyncNode_t& a_out) const
		{
			a_out.name  = a_in["name"].asString();
			a_out.flags = a_in["flags"].asUInt();

			return true;
		}

		template <>
		void Parser<Data::configExtraNodeEntrySkelTransformSyncNode_t>::Create(
			const Data::configExtraNodeEntrySkelTransformSyncNode_t& a_data,
			Json::Value&                                             a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}
	}
}
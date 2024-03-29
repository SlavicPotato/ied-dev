#include "pch.h"

#include "JSONConfigSkeletonMatchParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configSkeletonMatch_t>::Parse(
			const Json::Value&           a_in,
			Data::configSkeletonMatch_t& a_out) const
		{
			for (auto& f : a_in["ids"])
			{
				a_out.ids.emplace(f.asInt());
			}

			for (auto& f : a_in["sigs"])
			{
				a_out.signatures.emplace(f.asUInt64());
			}

			if (auto& d = a_in["any"])
			{
				a_out.matchAny = d.asBool();
			}

			return true;
		}

		template <>
		void Parser<Data::configSkeletonMatch_t>::Create(
			const Data::configSkeletonMatch_t& a_data,
			Json::Value&                       a_out) const
		{
			throw parser_exception(__FUNCTION__ ": " PARSER_NOT_IMPL_STR);
		}

	}
}
#include "pch.h"

#include "JSONConfigFixedStringSetParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFixedStringSet_t>::Parse(
			const Json::Value&            a_in,
			Data::configFixedStringSet_t& a_out) const
		{
			for (auto& e : a_in)
			{
				a_out.emplace(e.asString());
			}

			return true;
		}

		template <>
		void Parser<Data::configFixedStringSet_t>::Create(
			const Data::configFixedStringSet_t& a_data,
			Json::Value&                        a_out) const
		{
			for (auto& e : a_data)
			{
				a_out.append(*e);
			}
		}
	}
}
#include "pch.h"

#include "JSONConfigFixedStringListParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFixedStringList_t>::Parse(
			const Json::Value&             a_in,
			Data::configFixedStringList_t& a_out) const
		{
			for (auto& e : a_in)
			{
				a_out.emplace_back(e.asString());
			}

			return true;
		}

		template <>
		void Parser<Data::configFixedStringList_t>::Create(
			const Data::configFixedStringList_t& a_data,
			Json::Value&                         a_out) const
		{
			for (auto& e : a_data)
			{
				a_out.append(*e);
			}
		}
	}
}
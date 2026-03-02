#include "pch.h"

#include "JSONConfigObjectExtraTypeListParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configObjectTypeExtraList_t>::Parse(
			const Json::Value&             a_in,
			Data::configObjectTypeExtraList_t& a_out) const
		{
			for (auto& e : a_in)
			{
				a_out.emplace_back(static_cast<Data::ObjectTypeExtra>(e.asUInt()));
			}

			return true;
		}

		template <>
		void Parser<Data::configObjectTypeExtraList_t>::Create(
			const Data::configObjectTypeExtraList_t& a_in,
			Json::Value&                         a_out) const
		{
			auto& out = (a_out = Json::Value(Json::ValueType::arrayValue));
			for (auto& e : a_in)
			{
				out.append(stl::underlying(e));
			}
		}

	}
}
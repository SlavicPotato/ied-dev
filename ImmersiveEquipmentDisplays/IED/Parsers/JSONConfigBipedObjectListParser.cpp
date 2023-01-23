#include "pch.h"

#include "JSONConfigBipedObjectListParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configBipedObjectList_t>::Parse(
			const Json::Value&             a_in,
			Data::configBipedObjectList_t& a_out) const
		{
			for (auto& e : a_in)
			{
				a_out.emplace_back(static_cast<BIPED_OBJECT>(e.asUInt()));
			}

			return true;
		}

		template <>
		void Parser<Data::configBipedObjectList_t>::Create(
			const Data::configBipedObjectList_t& a_in,
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
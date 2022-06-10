#include "pch.h"

#include "JSONConfigSlotPriorityParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configSlotPriority_t>::Parse(
			const Json::Value&          a_in,
			Data::configSlotPriority_t& a_out) const
		{
			a_out.flags = a_in.get("flags", stl::underlying(Data::configSlotPriority_t::DEFAULT_FLAGS)).asUInt();
			a_out.limit = a_in.get("limit", stl::underlying(Data::ObjectType::kMax)).asUInt();

			auto& order = a_in["order"];

			using enum_type = std::underlying_type_t<Data::ObjectType>;

			for (enum_type i = 0; i < stl::underlying(Data::ObjectType::kMax); i++)
			{
				a_out.order[i] = static_cast<Data::ObjectType>(order.get(i, i).asUInt());
			}

			return true;
		}

		template <>
		void Parser<Data::configSlotPriority_t>::Create(
			const Data::configSlotPriority_t& a_in,
			Json::Value&                      a_out) const
		{
			a_out["flags"] = a_in.flags.underlying();
			a_out["limit"] = a_in.limit;

			auto& order = (a_out["order"] = Json::Value(Json::ValueType::arrayValue));

			using enum_type = std::underlying_type_t<Data::ObjectType>;

			for (enum_type i = 0; i < stl::underlying(Data::ObjectType::kMax); i++)
			{
				order[i] = stl::underlying(a_in.order[i]);
			}
		}

	}
}
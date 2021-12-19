#include "pch.h"

#include "JSONRangeParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configRange_t>::Parse(
			const Json::Value& a_in,
			Data::configRange_t& a_out) const
		{
			a_out.min = a_in.get("min", 0u).asUInt();
			a_out.max = a_in.get("max", 0u).asUInt();

			return true;
		}

		template <>
		void Parser<Data::configRange_t>::Create(
			const Data::configRange_t& a_data,
			Json::Value& a_out) const
		{
			a_out["min"] = a_data.min;
			a_out["max"] = a_data.max;
		}

		template <>
		void Parser<Data::configRange_t>::GetDefault(Data::configRange_t& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED
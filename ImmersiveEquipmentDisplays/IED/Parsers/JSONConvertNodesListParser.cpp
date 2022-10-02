#include "pch.h"

#include "JSONConvertNodesListParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<std::list<std::int32_t>>::Parse(
			const Json::Value&       a_in,
			std::list<std::int32_t>& a_out) const
		{
			for (auto& e : a_in)
			{
				for (auto& f : e["ids"])
				{
					a_out.emplace_back(f.asInt());
				}
			}

			return true;
		}

		template <>
		void Parser<std::list<std::int32_t>>::Create(
			const std::list<std::int32_t>& a_data,
			Json::Value&                   a_out) const
		{
			throw std::runtime_error("not implemented");
		}

	}
}
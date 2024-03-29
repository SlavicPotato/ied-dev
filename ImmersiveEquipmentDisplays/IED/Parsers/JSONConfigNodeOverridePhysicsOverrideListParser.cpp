#include "pch.h"

#include "JSONConfigNodeOverridePhysicsOverrideListParser.h"

#include "JSONConfigNodeOverridePhysicsOverrideParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configNodeOverridePhysicsOverrideList_t>::Parse(
			const Json::Value&                               a_in,
			Data::configNodeOverridePhysicsOverrideList_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::configNodeOverridePhysicsOverride_t> parser(m_state);

			for (auto& e : data)
			{
				if (!parser.Parse(e, a_out.emplace_back(), version))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverridePhysicsOverrideList_t>::Create(
			const Data::configNodeOverridePhysicsOverrideList_t& a_data,
			Json::Value&                                           a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::configNodeOverridePhysicsOverride_t> parser(m_state);

			for (auto& e : a_data)
			{
				parser.Create(e, data.append(Json::Value()));
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}
#include "pch.h"

#include "JSONConfigNodeOverridePhysicsOverrideParser.h"

#include "JSONConfigNodeOverrideConditionListParser.h"
#include "JSONConfigNodePhysicsValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePhysicsOverride_t>::Parse(
			const Json::Value&                           a_in,
			Data::configNodeOverridePhysicsOverride_t& a_out,
			const std::uint32_t                          a_version) const
		{
			Parser<Data::configNodePhysicsValues_t> vparser(m_state);

			if (!vparser.Parse(a_in["vals"], a_out))
			{
				return false;
			}

			if (auto& matches = a_in["cond"])
			{
				Parser<Data::configNodeOverrideConditionList_t> mlparser(m_state);

				if (!mlparser.Parse(matches, a_out.conditions))
				{
					return false;
				}
			}

			a_out.overrideFlags = static_cast<Data::NodeOverridePhysicsOverrideFlags>(
				a_in.get("override_flags", stl::underlying(Data::NodeOverridePhysicsOverrideFlags::kNone)).asUInt());

			a_out.description = a_in.get("desc", "").asString();

			return true;
		}

		template <>
		void Parser<Data::configNodeOverridePhysicsOverride_t>::Create(
			const Data::configNodeOverridePhysicsOverride_t& a_data,
			Json::Value&                                       a_out) const
		{
			Parser<Data::configNodePhysicsValues_t> vparser(m_state);

			vparser.Create(a_data, a_out["vals"]);

			if (!a_data.conditions.empty())
			{
				Parser<Data::configNodeOverrideConditionList_t> mlparser(m_state);

				mlparser.Create(a_data.conditions, a_out["cond"]);
			}

			a_out["override_flags"] = stl::underlying(a_data.overrideFlags.value);
			a_out["desc"]           = a_data.description;
		}

	}
}
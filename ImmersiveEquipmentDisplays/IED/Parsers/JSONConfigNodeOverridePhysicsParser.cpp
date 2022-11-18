#include "pch.h"

#include "JSONConfigNodeOverridePhysicsParser.h"

#include "JSONConfigNodeOverridePhysicsOverrideListParser.h"
#include "JSONConfigNodePhysicsValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverridePhysics_t>::Parse(
			const Json::Value&                   a_in,
			Data::configNodeOverridePhysics_t& a_out,
			const std::uint32_t                  a_version) const
		{
			Parser<Data::configNodePhysicsValues_t> vparser(m_state);

			if (!vparser.Parse(a_in["vals"], a_out))
			{
				return false;
			}

			if (auto& overrides = a_in["overrides"])
			{
				Parser<Data::configNodeOverridePhysicsOverrideList_t> lparser(m_state);

				if (!lparser.Parse(overrides, a_out.overrides))
				{
					return false;
				}
			}

			a_out.flags = static_cast<Data::NodeOverridePhysicsFlags>(
				a_in.get("parent_flags", stl::underlying(Data::NodeOverridePhysicsFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configNodeOverridePhysics_t>::Create(
			const Data::configNodeOverridePhysics_t& a_data,
			Json::Value&                               a_out) const
		{
			Parser<Data::configNodePhysicsValues_t> vparser(m_state);

			vparser.Create(a_data, a_out["vals"]);

			if (!a_data.overrides.empty())
			{
				Parser<Data::configNodeOverridePhysicsOverrideList_t> lparser(m_state);

				lparser.Create(a_data.overrides, a_out["overrides"]);
			}

			a_out["parent_flags"] = stl::underlying(a_data.flags.value);
		}

	}
}
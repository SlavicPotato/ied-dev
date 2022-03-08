#include "pch.h"

#include "JSONConfigNodeOverrideValuesParser.h"
#include "JSONConfigTransformParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideValues_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideValues_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configTransform_t> tfparser(m_state);

			a_out.flags = static_cast<Data::NodeOverrideValuesFlags>(
				a_in.get("flags", stl::underlying(Data::NodeOverrideValuesFlags::kNone)).asUInt());

			if (!tfparser.Parse(a_in, a_out.transform, a_version))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideValues_t>::Create(
			const Data::configNodeOverrideValues_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Data::configTransform_t> tfparser(m_state);

			a_out["flags"] = stl::underlying(a_data.flags.value);

			tfparser.Create(a_data.transform, a_out);
		}

	}
}
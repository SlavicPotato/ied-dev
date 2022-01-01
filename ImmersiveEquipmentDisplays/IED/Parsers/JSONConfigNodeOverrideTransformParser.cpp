#include "pch.h"

#include "JSONConfigNodeOverrideOffsetListParser.h"
#include "JSONConfigNodeOverrideConditionListParser.h"
#include "JSONConfigNodeOverrideTransformParser.h"
#include "JSONConfigNodeOverrideValuesParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideTransform_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideTransform_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configNodeOverrideValues_t> vparser(m_state);
			Parser<Data::configNodeOverrideOffsetList_t> lparser(m_state);
			Parser<Data::configNodeOverrideConditionList_t> mlparser(m_state);

			if (!vparser.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			if (!lparser.Parse(a_in["offsets"], a_out.offsets))
			{
				return false;
			}

			if (!mlparser.Parse(a_in["vis_ml"], a_out.visibilityConditionList))
			{
				return false;
			}

			a_out.overrideFlags = static_cast<Data::NodeOverrideFlags>(
				a_in.get("override_flags", stl::underlying(Data::NodeOverrideFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideTransform_t>::Create(
			const Data::configNodeOverrideTransform_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Data::configNodeOverrideValues_t> vparser(m_state);
			Parser<Data::configNodeOverrideOffsetList_t> lparser(m_state);
			Parser<Data::configNodeOverrideConditionList_t> mlparser(m_state);

			vparser.Create(a_data, a_out);
			lparser.Create(a_data.offsets, a_out["offsets"]);
			mlparser.Create(a_data.visibilityConditionList, a_out["vis_ml"]);

			a_out["override_flags"] = stl::underlying(a_data.overrideFlags.value);
		}

		template <>
		void Parser<Data::configNodeOverrideTransform_t>::GetDefault(
			Data::configNodeOverrideTransform_t& a_out) const
		{
		}
	}
}
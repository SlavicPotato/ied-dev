#include "pch.h"

#include "JSONEquipmentOverrideListParser.h"
#include "JSONConfigBaseParser.h"
#include "JSONConfigBaseValuesParser.h"
#include "JSONFormFilterParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configBase_t>::Parse(
			const Json::Value& a_in,
			Data::configBase_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configBaseValues_t> bvParser;
			Parser<Data::equipmentOverrideList_t> aoListParser;
			Parser<Data::configFormFilter_t> pfset;

			if (!bvParser.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			auto& aoData = a_in["ao"];

			if (!aoData.empty())
			{
				if (!aoListParser.Parse(aoData, a_out.equipmentOverrides))
				{
					return false;
				}
			}

			pfset.Parse(a_in["rfilter"], a_out.raceFilter);

			return true;
		}

		template <>
		void Parser<Data::configBase_t>::Create(
			const Data::configBase_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Data::configBaseValues_t> bvParser;
			Parser<Data::equipmentOverrideList_t> aoListParser;
			Parser<Data::configFormFilter_t> pfset;

			bvParser.Create(a_data, a_out);
			if (!a_data.equipmentOverrides.empty())
			{
				aoListParser.Create(a_data.equipmentOverrides, a_out["ao"]);
			}

			pfset.Create(a_data.raceFilter, a_out["rfilter"]);
		}

		template <>
		void Parser<Data::configBase_t>::GetDefault(Data::configBase_t& a_out) const
		{}
	}  // namespace Serialization
}  // namespace IED
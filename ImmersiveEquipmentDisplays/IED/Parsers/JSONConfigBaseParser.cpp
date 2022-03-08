#include "pch.h"

#include "JSONConfigBaseParser.h"
#include "JSONConfigBaseValuesParser.h"
#include "JSONEffectShaderListParser.h"
#include "JSONEquipmentOverrideListParser.h"
#include "JSONFormFilterParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configBase_t>::Parse(
			const Json::Value&  a_in,
			Data::configBase_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configBaseValues_t> bvParser(m_state);

			if (!bvParser.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			if (auto& ao = a_in["ao"])
			{
				Parser<Data::equipmentOverrideList_t> aoListParser(m_state);

				if (!aoListParser.Parse(ao, a_out.equipmentOverrides))
				{
					return false;
				}
			}

			if (auto& filtData = a_in["bflt"])
			{
				Parser<Data::configFormFilter_t> pfset(m_state);

				a_out.filters = std::make_unique<Data::configBaseFilters_t>();

				if (!pfset.Parse(filtData["r"], a_out.filters->raceFilter))
				{
					return false;
				}

				if (!pfset.Parse(filtData["a"], a_out.filters->actorFilter))
				{
					return false;
				}

				if (!pfset.Parse(filtData["n"], a_out.filters->npcFilter))
				{
					return false;
				}
			}

			if (auto& esl = a_in["esl"])
			{
				Parser<Data::effectShaderList_t> eslist(m_state);

				if (!eslist.Parse(esl, a_out.effectShaders))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configBase_t>::Create(
			const Data::configBase_t& a_data,
			Json::Value&              a_out) const
		{
			Parser<Data::configBaseValues_t> bvParser(m_state);

			bvParser.Create(a_data, a_out);
			if (!a_data.equipmentOverrides.empty())
			{
				Parser<Data::equipmentOverrideList_t> aoListParser(m_state);

				aoListParser.Create(a_data.equipmentOverrides, a_out["ao"]);
			}

			if (a_data.filters)
			{
				Parser<Data::configFormFilter_t> pfset(m_state);

				auto& filtData = (a_out["bflt"] = Json::Value(Json::ValueType::objectValue));

				pfset.Create(a_data.filters->raceFilter, filtData["r"]);
				pfset.Create(a_data.filters->actorFilter, filtData["a"]);
				pfset.Create(a_data.filters->npcFilter, filtData["n"]);
			}

			if (!a_data.effectShaders.empty())
			{
				Parser<Data::effectShaderList_t> eslist(m_state);

				eslist.Create(a_data.effectShaders, a_out["esl"]);
			}
		}

	}
}
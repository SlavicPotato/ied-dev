#include "pch.h"

#include "JSONConfigBaseParser.h"
#include "JSONConfigCachedFormParser.h"
#include "JSONConfigCustomParser.h"
#include "JSONConfigOverrideModelGroupParser.h"
#include "JSONEquipmentOverrideConditionListParser.h"
#include "JSONFormListParser.h"
#include "JSONFormParser.h"
#include "JSONRangeParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configCustom_t>::Parse(
			const Json::Value&    a_in,
			Data::configCustom_t& a_out,
			const std::uint32_t   a_version) const
		{
			Parser<Data::configBase_t>                     pbase(m_state);
			Parser<Data::configRange_t>                    prange(m_state);
			Parser<Data::configCachedForm_t>               pform(m_state);
			Parser<Data::configModelGroup_t>               gparser(m_state);

			if (!pbase.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			pform.Parse(a_in["item"], a_out.form);
			pform.Parse(a_in["model"], a_out.modelForm);

			if (!prange.Parse(a_in["cr"], a_out.countRange))
			{
				return false;
			}

			if (auto& extra = a_in["extra"])
			{
				Parser<Data::configFormList_t> pformList(m_state);

				if (!pformList.Parse(extra, a_out.extraItems, a_version))
				{
					return false;
				}
			}

			if (auto& bsl = a_in["bsl"])
			{
				for (auto& e : bsl)
				{
					auto v = e.asUInt();
					if (v >= stl::underlying(BIPED_OBJECT::kTotal))
					{
						Error("%s: bad biped slot index", __FUNCTION__);
						return false;
					}

					a_out.bipedSlots.emplace_back(static_cast<BIPED_OBJECT>(v));
				}
			}

			if (auto& bfc = a_in["bfc"])
			{
				Parser<Data::equipmentOverrideConditionList_t> cparser(m_state);

				if (!cparser.Parse(bfc, a_out.bipedFilterConditions))
				{
					return false;
				}
			}

			if (!gparser.Parse(a_in["mgrp"], a_out.group))
			{
				return false;
			}

			a_out.customFlags = a_in.get("cflags", stl::underlying(Data::configCustom_t::DEFAULT_CUSTOM_FLAGS)).asUInt();

			//a_out.priority = a_in.get("prio", 0u).asUInt();
			a_out.chance = a_in.get("chance", 100.0f).asFloat();

			return true;
		}

		template <>
		void Parser<Data::configCustom_t>::Create(
			const Data::configCustom_t& a_in,
			Json::Value&                a_out) const
		{
			Parser<Data::configBase_t>                     pbase(m_state);
			Parser<Data::configCachedForm_t>               pform(m_state);
			Parser<Data::configModelGroup_t>               gparser(m_state);

			pbase.Create(a_in, a_out);

			if (a_in.form.get_id())
			{
				pform.Create(a_in.form.get_id(), a_out["item"]);
			}

			if (a_in.modelForm.get_id())
			{
				pform.Create(a_in.modelForm.get_id(), a_out["model"]);
			}

			if (!a_in.countRange.empty())
			{
				Parser<Data::configRange_t> prange(m_state);

				prange.Create(a_in.countRange, a_out["cr"]);
			}

			if (!a_in.extraItems.empty())
			{
				Parser<Data::configFormList_t> pformList(m_state);

				pformList.Create(a_in.extraItems, a_out["extra"]);
			}

			if (!a_in.bipedSlots.empty())
			{
				auto& out = (a_out["bsl"] = Json::Value(Json::ValueType::arrayValue));
				for (auto& e : a_in.bipedSlots)
				{
					out.append(stl::underlying(e));
				}
			}

			if (!a_in.bipedFilterConditions.empty())
			{
				Parser<Data::equipmentOverrideConditionList_t> cparser(m_state);

				cparser.Create(a_in.bipedFilterConditions, a_out["bfc"]);
			}

			gparser.Create(a_in.group, a_out["mgrp"]);

			a_out["cflags"] = a_in.customFlags.underlying();
			//a_out["prio"] = a_in.priority;
			a_out["chance"] = a_in.chance;
		}

	}
}
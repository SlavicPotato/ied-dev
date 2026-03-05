#include "pch.h"

#include "JSONConfigBaseParser.h"
#include "JSONConfigBipedObjectListParser.h"
#include "JSONConfigCachedFormParser.h"
#include "JSONConfigCustomParser.h"
#include "JSONConfigFixedStringListParser.h"
#include "JSONConfigInventoryParser.h"
#include "JSONConfigLastEquippedParser.h"
#include "JSONConfigOverrideModelGroupParser.h"
#include "JSONConfigVariableSource.h"
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
			Parser<Data::configBase_t>       pbase(m_state);
			Parser<Data::configRange_t>      prange(m_state);
			Parser<Data::configCachedForm_t> pform(m_state);
			Parser<Data::configModelGroup_t> gparser(m_state);

			if (!pbase.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			pform.Parse(a_in["item"], a_out.form);
			pform.Parse(a_in["model"], a_out.modelForm);

			if (a_version >= 4)
			{
				Parser<Data::configInventory_t> pinv(m_state);

				if (!pinv.Parse(a_in["inv"], a_out.inv))
				{
					return false;
				}
			}
			else
			{
				if (auto& cr = a_in["cr"])
				{
					if (!prange.Parse(cr, a_out.inv.countRange))
					{
						return false;
					}
				}

				if (auto& extra = a_in["extra"])
				{
					Parser<Data::configFormList_t> pformList(m_state);

					if (!pformList.Parse(extra, a_out.inv.extraItems, a_version))
					{
						return false;
					}
				}

				a_out.move_custom_flags_to_inv();
			}

			a_out.probability = a_in.get("chance", 100.0f).asFloat();

			if (a_version >= 3)
			{
				Parser<Data::configLastEquipped_t> leparser(m_state);

				if (!leparser.Parse(a_in["leqp"], a_out.lastEquipped))
				{
					return false;
				}
			}
			else
			{
				if (auto& bsl = a_in["bsl"])
				{
					Parser<Data::configBipedObjectList_t> parser(m_state);

					if (!parser.Parse(bsl, a_out.lastEquipped.bipedSlots))
					{
						return false;
					}
				}

				if (auto& bfc = a_in["bfc"])
				{
					Parser<Data::equipmentOverrideConditionList_t> parser(m_state);

					if (!parser.Parse(bfc, a_out.lastEquipped.filterConditions.list))
					{
						return false;
					}
				}

				a_out.move_legacy_flags_to_le();
			}

			if (!gparser.Parse(a_in["mgrp"], a_out.group))
			{
				return false;
			}

			Parser<Data::configVariableSource_t> vsparser(m_state);

			if (!vsparser.Parse(a_in["vsrc"], a_out.vss.varSource))
			{
				return false;
			}

			Parser<Data::configFixedStringList_t> fslparser(m_state);

			if (!fslparser.Parse(a_in["fvars"], a_out.vss.formVars))
			{
				return false;
			}

			a_out.customFlags = a_in.get("cflags", stl::underlying(Data::configCustom_t::DEFAULT_CUSTOM_FLAGS)).asUInt();

			return true;
		}

		template <>
		void Parser<Data::configCustom_t>::Create(
			const Data::configCustom_t& a_in,
			Json::Value&                a_out) const
		{
			Parser<Data::configBase_t>       pbase(m_state);
			Parser<Data::configCachedForm_t> pform(m_state);
			Parser<Data::configModelGroup_t> gparser(m_state);

			pbase.Create(a_in, a_out);

			if (a_in.form.get_id())
			{
				pform.Create(a_in.form.get_id(), a_out["item"]);
			}

			if (a_in.modelForm.get_id())
			{
				pform.Create(a_in.modelForm.get_id(), a_out["model"]);
			}

			Parser<Data::configInventory_t> pinv(m_state);

			pinv.Create(a_in.inv, a_out["inv"]);

			Parser<Data::configLastEquipped_t> leparser(m_state);

			leparser.Create(a_in.lastEquipped, a_out["leqp"]);

			Parser<Data::configVariableSource_t> vsparser(m_state);

			vsparser.Create(a_in.vss.varSource, a_out["vsrc"]);

			Parser<Data::configFixedStringList_t> fslparser(m_state);

			fslparser.Create(a_in.vss.formVars, a_out["fvars"]);

			gparser.Create(a_in.group, a_out["mgrp"]);

			a_out["chance"] = a_in.probability;

			a_out["cflags"] = a_in.customFlags.underlying();
			//a_out["prio"] = a_in.priority;
		}

	}
}
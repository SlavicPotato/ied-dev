#include "pch.h"

#include "JSONConfigBaseParser.h"
#include "JSONConfigSlotParser.h"
#include "JSONEquipmentOverrideConditionListParser.h"
#include "JSONFormFilterParser.h"
#include "JSONFormListParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configSlot_t>::Parse(
			const Json::Value&  a_in,
			Data::configSlot_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configBase_t>       pbase(m_state);
			Parser<Data::configFormFilter_t> pfset(m_state);

			if (!pbase.Parse(a_in, a_out, a_version))
			{
				return false;
			}

			if (auto& pil = a_in["pil"])
			{
				Parser<Data::configFormList_t> pformList(m_state);

				if (!pformList.Parse(pil, a_out.preferredItems, a_version))
				{
					return false;
				}
			}

			if (!pfset.Parse(a_in["iflt"], a_out.itemFilter))
			{
				return false;
			}

			if (auto& d = a_in["ifc"])
			{
				Parser<Data::equipmentOverrideConditionList_t> parser(m_state);

				if (!parser.Parse(d, a_out.itemFilterCondition))
				{
					return false;
				}
			}

			a_out.slotFlags = static_cast<Data::SlotFlags>(
				a_in.get("sflags", stl::underlying(Data::configSlot_t::DEFAULT_SLOT_FLAGS)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configSlot_t>::Create(
			const Data::configSlot_t& a_in,
			Json::Value&              a_out) const
		{
			Parser<Data::configBase_t>           pbase(m_state);
			Parser<Data::configFormList_t>       pformList(m_state);
			Parser<Data::configFormFilter_t>     pfset(m_state);
			Parser<Data::configFixedStringSet_t> fssparser(m_state);

			pbase.Create(a_in, a_out);

			if (!a_in.preferredItems.empty())
			{
				pformList.Create(a_in.preferredItems, a_out["pil"]);
			}

			pfset.Create(a_in.itemFilter, a_out["iflt"]);

			if (!a_in.itemFilterCondition.empty())
			{
				Parser<Data::equipmentOverrideConditionList_t> parser(m_state);

				parser.Create(a_in.itemFilterCondition, a_out["ifc"]);
			}

			a_out["sflags"] = stl::underlying(a_in.slotFlags.value);
		}

	}
}
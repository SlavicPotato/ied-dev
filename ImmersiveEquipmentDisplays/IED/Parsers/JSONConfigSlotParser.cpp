#include "pch.h"

#include "JSONConfigBaseParser.h"
#include "JSONConfigSlotParser.h"
#include "JSONFormFilterParser.h"
#include "JSONFormListParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configSlot_t>::Parse(
			const Json::Value& a_in,
			Data::configSlot_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Data::configBase_t> pbase;
			Parser<Data::configFormList_t> pformList;
			Parser<Data::configFormFilter_t> pfset;

			pbase.Parse(a_in, a_out, a_version);
			pformList.Parse(a_in["pil"], a_out.preferredItems, a_version);
			pfset.Parse(a_in["flt"], a_out.itemFilter);

			a_out.slotFlags = static_cast<Data::SlotFlags>(
				a_in.get("sflags", stl::underlying(Data::configSlot_t::DEFAULT_SLOT_FLAGS)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configSlot_t>::Create(
			const Data::configSlot_t& a_in,
			Json::Value& a_out) const
		{
			Parser<Data::configBase_t> pbase;
			Parser<Data::configFormList_t> pformList;
			Parser<Data::configFormFilter_t> pfset;

			pbase.Create(a_in, a_out);
			if (!a_in.preferredItems.empty())
			{
				pformList.Create(a_in.preferredItems, a_out["pil"]);
			}

			pfset.Create(a_in.itemFilter, a_out["flt"]);

			a_out["sflags"] = stl::underlying(a_in.slotFlags.value);
		}

		template <>
		void Parser<Data::configSlot_t>::GetDefault(Data::configSlot_t& a_out) const
		{}
	}  // namespace Serialization
}  // namespace IED
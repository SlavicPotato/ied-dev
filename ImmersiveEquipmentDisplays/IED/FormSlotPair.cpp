#include "pch.h"

#include "FormSlotPair.h"

#include "Data.h"

namespace IED
{

	FormSlotPair::FormSlotPair(
		TESForm* a_form) noexcept :
		form(a_form)
	{
		const auto exslot = Data::ItemData::GetItemSlotExtraGeneric(a_form);

		slot  = exslot;
		slot2 = Data::ItemData::ExtraSlotToSlot(exslot);
	}

	FormSlotPair::FormSlotPair(
		TESForm*              a_form,
		Data::ObjectSlotExtra a_slot) noexcept :
		form(a_form),
		slot(a_slot),
		slot2(Data::ItemData::ExtraSlotToSlot(a_slot))
	{
	}

}
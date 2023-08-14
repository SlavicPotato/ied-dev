#pragma once

#include "ConfigData.h"

namespace IED
{
	struct FormSlotPair
	{
		FormSlotPair(
			TESForm* a_form) noexcept;

		FormSlotPair(
			TESForm*              a_form,
			Data::ObjectSlotExtra a_slot) noexcept;

		constexpr FormSlotPair(
			TESForm*              a_form,
			Data::ObjectSlotExtra a_slotex,
			Data::ObjectSlot      a_slot) noexcept :
			form(a_form),
			slot(a_slotex),
			slot2(a_slot)
		{
		}

		TESForm*              form;
		Data::ObjectSlotExtra slot;
		Data::ObjectSlot      slot2;
	};

}
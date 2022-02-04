#pragma once

#include "CommonParams.h"
#include "ConfigOverrideCommon.h"

#include "IED/Data.h"

namespace IED
{
	namespace Conditions
	{
		bool match_extra(
			CommonParams& a_params,
			Data::ExtraConditionType a_type);

		bool match_form(
			Game::FormID a_formid,
			TESForm* a_form);

		SKMP_FORCEINLINE constexpr bool is_hand_slot(Data::ObjectSlotExtra a_slot)
		{
			return a_slot != Data::ObjectSlotExtra::kArmor &&
			       a_slot != Data::ObjectSlotExtra::kAmmo;
		}

		SKMP_FORCEINLINE constexpr bool is_valid_form_for_slot(
			TESForm* a_form,
			Data::ObjectSlotExtra a_slot,
			bool a_left)
		{
			return a_left ?
                       Data::ItemData::GetItemSlotLeftExtra(a_form) == a_slot :
                       Data::ItemData::GetItemSlotExtra(a_form) == a_slot;
		}

	}
}
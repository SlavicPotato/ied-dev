#pragma once

#include "Localization/Common.h"

namespace IED
{
	namespace UI
	{
		enum class UICustomEditorString : Localization::StringID
		{
			IsInventoryItem          = 4200,
			SelectedItemNotInventory = 4201,
			EquipmentMode            = 4202,
			IgnoreRaceEquipTypes     = 4203,
			DisableIfEquipped        = 4204,
			ModelSwap                = 4205,
			LoadARMA                 = 4206,
			ModelGroup               = 4207,
			PrioritizeRecentSlots    = 4208,
			DisableIfOccupied        = 4209,
			LastEquippedFilterCond   = 4210,
			SkipOccupiedSlots        = 4211,
		};
	}
}
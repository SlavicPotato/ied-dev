#pragma once

#include "IED/ConfigEquipment.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

namespace IED
{
	namespace UI
	{

		enum class UIEquipmentOverrideAction
		{
			None,
			Insert,
			InsertGroup,
			Delete,
			Swap,
			Edit,
			Reset,
			Rename,
			Copy,
			Paste,
			PasteOver,
			Create
		};

		struct UIEquipmentOverrideResult
		{
			UIEquipmentOverrideAction            action{ UIEquipmentOverrideAction::None };
			Game::FormID                         form;
			Data::EquipmentOverrideConditionType entryType;

			union
			{
				BIPED_OBJECT             biped;
				Data::ExtraConditionType excond;
				Data::ObjectSlotExtra    slot;
			};

			std::string   desc;
			SwapDirection dir{ SwapDirection::None };
		};

	}
}
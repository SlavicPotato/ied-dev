#pragma once

#include "IED/ConfigEquipment.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

namespace IED
{
	namespace UI
	{

		enum class BaseConfigEditorAction : std::uint8_t
		{
			None,
			Insert,
			InsertGroup,
			Delete,
			Swap,
			Edit,
			Reset,
			Rename,
			ClearKeyword,
			Copy,
			Paste,
			PasteOver,
			Create
		};

		struct EquipmentOverrideResult
		{
			BaseConfigEditorAction               action{ BaseConfigEditorAction::None };
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
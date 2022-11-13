#pragma once

#include "IED/ConfigEquipment.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "BaseConfigEditorAction.h"

namespace IED
{
	namespace UI
	{

		struct UIEquipmentOverrideResult
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
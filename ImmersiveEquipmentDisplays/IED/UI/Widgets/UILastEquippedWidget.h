#pragma once

#include "UIEquipmentOverrideConditionsWidget.h"

#include "Lists/UIBipedObjectList.h"
#include "Lists/UIObjectSlotList.h"

#include "IED/ConfigLastEquipped.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UILastEquippedWidget :
			public UIBipedObjectList,
			public UIObjectSlotList,
			public virtual UIEquipmentOverrideConditionsWidget
		{
		public:
			UILastEquippedWidget(Controller& a_controller);

			void DrawLastEquippedPanel(
				Data::configLastEquipped_t& a_data,
				update_func_t               a_updateFunc);
		};
	}
}
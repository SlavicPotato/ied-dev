#pragma once

#include "IED/ConfigCommon.h"

#include "UISlotEditorActor.h"
#include "UISlotEditorGlobal.h"
#include "UISlotEditorNPC.h"
#include "UISlotEditorRace.h"

#include "IED/UI/UIEditorTabPanel.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISlotEditorTabPanel :
			public UIEditorTabPanel
		{
		public:
			static constexpr auto PANEL_ID = UIDisplayManagementEditorPanel::Slot;

			UISlotEditorTabPanel(Controller& a_controller);

		private:
			virtual Data::SettingHolder::EditorPanel& GetEditorConfig() override;
		};

	}
}
#include "pch.h"

#include "UISlotTabPanel.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/UIMainStrings.h"

namespace IED
{
	namespace UI
	{
		UISlotEditorTabPanel::UISlotEditorTabPanel(Controller& a_controller) :
			UIEditorTabPanel(
				a_controller,
				UIMainStrings::EquipmentDisplays,
				Interface{ std::make_unique<UISlotEditorRace>(a_controller), CommonStrings::Race },
				Interface{ std::make_unique<UISlotEditorNPC>(a_controller), CommonStrings::NPC },
				Interface{ std::make_unique<UISlotEditorActor>(a_controller), CommonStrings::Actor },
				Interface{ std::make_unique<UISlotEditorGlobal>(a_controller), CommonStrings::Global })
		{
		}

		Data::SettingHolder::EditorPanel& UISlotEditorTabPanel::GetEditorConfig()
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor;
		}

	}
}
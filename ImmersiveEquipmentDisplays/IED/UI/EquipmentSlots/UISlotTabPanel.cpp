#include "pch.h"

#include "UISlotTabPanel.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UISlotEditorTabPanel::UISlotEditorTabPanel(Controller& a_controller) :
			UIEditorTabPanel(a_controller, stl::underlying(CommonStrings::Equipment)),
			UILocalizationInterface(a_controller),
			m_slotEditorActor(a_controller),
			m_slotEditorRace(a_controller),
			m_slotEditorGlobal(a_controller),
			m_slotEditorNPC(a_controller),
			m_controller(a_controller)
		{
			SetEditor(Data::ConfigClass::Global, m_slotEditorGlobal, stl::underlying(CommonStrings::Global));
			SetEditor(Data::ConfigClass::Actor, m_slotEditorActor, stl::underlying(CommonStrings::Actor));
			SetEditor(Data::ConfigClass::NPC, m_slotEditorNPC, stl::underlying(CommonStrings::NPC));
			SetEditor(Data::ConfigClass::Race, m_slotEditorRace, stl::underlying(CommonStrings::Race));
		}

		Data::SettingHolder::EditorPanel& UISlotEditorTabPanel::GetEditorConfig()
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor;
		}

	}
}
#include "pch.h"

#include "UICustomTabPanel.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/UIMainStrings.h"

namespace IED
{
	namespace UI
	{
		UICustomEditorTabPanel::UICustomEditorTabPanel(Controller& a_controller) :
			UIEditorTabPanel(a_controller, stl::underlying(UIMainStrings::CustomDisplays)),
			UILocalizationInterface(a_controller),
			m_editorActor(a_controller),
			m_editorRace(a_controller),
			m_editorGlobal(a_controller),
			m_editorNPC(a_controller),
			m_controller(a_controller)
		{
			SetEditor(Data::ConfigClass::Global, m_editorGlobal, stl::underlying(CommonStrings::Global));
			SetEditor(Data::ConfigClass::Actor, m_editorActor, stl::underlying(CommonStrings::Actor));
			SetEditor(Data::ConfigClass::Race, m_editorRace, stl::underlying(CommonStrings::Race));
			SetEditor(Data::ConfigClass::NPC, m_editorNPC, stl::underlying(CommonStrings::NPC));
		}

		Data::SettingHolder::EditorPanel& UICustomEditorTabPanel::GetEditorConfig()
		{
			return m_controller.GetConfigStore().settings.data.ui.customEditor;
		}

	}
}
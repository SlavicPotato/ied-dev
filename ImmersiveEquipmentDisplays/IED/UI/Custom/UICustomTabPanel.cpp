#include "pch.h"

#include "UICustomTabPanel.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/UIMainStrings.h"

namespace IED
{
	namespace UI
	{
		UICustomEditorTabPanel::UICustomEditorTabPanel(Controller& a_controller) :
			UIEditorTabPanel(
				a_controller,
				UIMainStrings::CustomDisplays,
				Interface{ std::make_unique<UICustomEditorRace>(a_controller), CommonStrings::Race },
				Interface{ std::make_unique<UICustomEditorNPC>(a_controller), CommonStrings::NPC },
				Interface{ std::make_unique<UICustomEditorActor>(a_controller), CommonStrings::Actor },
				Interface{ std::make_unique<UICustomEditorGlobal>(a_controller), CommonStrings::Global })
		{
		}

		Data::SettingHolder::EditorPanel& UICustomEditorTabPanel::GetEditorConfig()
		{
			return m_controller.GetConfigStore().settings.data.ui.customEditor;
		}

	}
}
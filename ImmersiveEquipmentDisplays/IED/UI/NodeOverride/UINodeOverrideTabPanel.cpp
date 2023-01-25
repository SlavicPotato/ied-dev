#include "pch.h"

#include "UINodeOverrideTabPanel.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideTabPanel::UINodeOverrideTabPanel(Controller& a_controller) :
			UIEditorTabPanel(
				a_controller,
				UIWidgetCommonStrings::GearPositioning,
				Interface{ std::make_unique<UINodeOverrideEditorRace>(a_controller), CommonStrings::Race },
				Interface{ std::make_unique<UINodeOverrideEditorNPC>(a_controller), CommonStrings::NPC },
				Interface{ std::make_unique<UINodeOverrideEditorActor>(a_controller), CommonStrings::Actor },
				Interface{ std::make_unique<UINodeOverrideEditorGlobal>(a_controller), CommonStrings::Global }
			)
		{
		}

		Data::SettingHolder::EditorPanel& UINodeOverrideTabPanel::GetEditorConfig()
		{
			return m_controller.GetSettings().data.ui.transformEditor;
		}

	}
}
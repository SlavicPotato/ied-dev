#include "pch.h"

#include "UIOutfitEditorTabPanel.h"

#include "UIOutfitEditorGlobal.h"
#include "UIOutfitEditorNPC.h"
#include "UIOutfitEditorRace.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/UIMainStrings.h"

namespace IED
{
	namespace UI
	{
		namespace OM
		{
			UIOutfitEditorTabPanel::UIOutfitEditorTabPanel(Controller& a_controller) :
				UIEditorTabPanel(
					a_controller,
					UIMainStrings::OutfitConfig,
					Interface{ std::make_unique<UIOutfitEditorRace>(a_controller), CommonStrings::Race },
					Interface{ std::make_unique<UIOutfitEditorNPC>(a_controller), CommonStrings::NPC },
					Interface{ std::unique_ptr<UIEditorInterface>(), CommonStrings::Actor },
					Interface{ std::unique_ptr<UIEditorInterface>(), CommonStrings::Global })
			{
			}

			Data::SettingHolder::EditorPanel& UIOutfitEditorTabPanel::GetEditorConfig()
			{
				return m_controller.GetSettings().data.ui.outfitEditor;
			}
		}
	}
}

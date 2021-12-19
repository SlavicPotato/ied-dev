#pragma once

#include "IED/ConfigCommon.h"

#include "UINodeOverrideEditorGlobal.h"
#include "UINodeOverrideEditorActor.h"
#include "UINodeOverrideEditorNPC.h"
#include "UINodeOverrideEditorRace.h"

#include "IED/UI/UIEditorTabPanel.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeOverrideTabPanel :
			public UIEditorTabPanel
		{
		public:
			UINodeOverrideTabPanel(Controller& a_controller);

			UINodeOverrideEditorGlobal m_editorGlobal;
			UINodeOverrideEditorActor m_editorActor;
			UINodeOverrideEditorNPC m_editorNPC;
			UINodeOverrideEditorRace m_editorRace;

		private:
			virtual Data::SettingHolder::EditorPanel& GetEditorConfig() override;

			Controller& m_controller;
		};

	}  // namespace UI
}  // namespace IED
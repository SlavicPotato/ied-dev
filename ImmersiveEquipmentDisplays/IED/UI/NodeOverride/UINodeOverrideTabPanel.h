#pragma once

#include "IED/ConfigCommon.h"

#include "UINodeOverrideEditorActor.h"
#include "UINodeOverrideEditorGlobal.h"
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

		private:
			virtual Data::SettingHolder::EditorPanel& GetEditorConfig() override;
		};

	}
}
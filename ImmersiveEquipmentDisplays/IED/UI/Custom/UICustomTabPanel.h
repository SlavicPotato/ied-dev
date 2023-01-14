#pragma once

#include "IED/ConfigCommon.h"

#include "UICustomEditorActor.h"
#include "UICustomEditorGlobal.h"
#include "UICustomEditorNPC.h"
#include "UICustomEditorRace.h"

#include "IED/UI/UIEditorTabPanel.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UICustomEditorTabPanel :
			public UIEditorTabPanel
		{
		public:
			static constexpr auto PANEL_ID = UIDisplayManagementEditorPanel::Custom;

			UICustomEditorTabPanel(Controller& a_controller);

		private:
			virtual Data::SettingHolder::EditorPanel& GetEditorConfig() override;
		};

	}
}
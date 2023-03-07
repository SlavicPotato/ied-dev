#pragma once

#include "IED/ConfigCommon.h"

#include "IED/UI/UIEditorTabPanel.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace OM
		{
			class UIOutfitEditorTabPanel :
				public UIEditorTabPanel
			{
			public:
				UIOutfitEditorTabPanel(Controller& a_controller);

			private:
				virtual Data::SettingHolder::EditorPanel& GetEditorConfig() override;
			};

		}
	}
}
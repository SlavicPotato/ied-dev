#pragma once

#include "IED/UI/UILocalizationInterface.h"
#include "UIEditorPanelSettings.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEditorPanelSettingsGear :
			public UIEditorPanelSettings
		{
		public:
			UIEditorPanelSettingsGear() = default;

		protected:
			virtual void DrawExtraEditorPanelSettings() override;
		};
	}
}
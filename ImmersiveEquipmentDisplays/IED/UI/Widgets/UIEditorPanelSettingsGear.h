#pragma once

#include "IED/UI/UILocalizationInterface.h"
#include "UIEditorPanelSettings.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEditorPanelSettingsGear :
			public UIEditorPanelSettings,
			public virtual UILocalizationInterface
		{
		public:
			UIEditorPanelSettingsGear(Controller& a_controller);

		protected:
			virtual void DrawExtraEditorPanelSettings() override;
		};
	}
}
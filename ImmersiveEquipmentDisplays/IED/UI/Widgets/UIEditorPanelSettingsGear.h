#pragma once

#include "UIEditorPanelSettings.h"
#include "IED/UI/UILocalizationInterface.h"

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
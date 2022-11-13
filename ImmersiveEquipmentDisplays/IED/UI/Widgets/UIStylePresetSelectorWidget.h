#pragma once

#include "ImGui/Styles/StylePreset.h"

#include "UIStylePresetSelectorWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UIStylePresetSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIStylePresetSelectorWidget(
				Localization::ILocalization& a_localization);

			bool DrawStylePresetSelector(
				UIStylePreset& a_preset);

			const char* preset_to_desc(UIStylePreset a_preset) const;
		};
	}

}

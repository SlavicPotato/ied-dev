#pragma once

#include "ImGui/Styles/StylePreset.h"

#include "Drivers/UI/StyleProfileManager.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UIStylePresetSelectorWidget
		{
		public:
			static bool DrawStylePresetSelector(
				const stl::vectormap<stl::fixed_string, StyleProfile>& a_data,
				stl::fixed_string&                                     a_preset);

			static const char* preset_to_desc(UIStylePreset a_preset);
		};
	}

}

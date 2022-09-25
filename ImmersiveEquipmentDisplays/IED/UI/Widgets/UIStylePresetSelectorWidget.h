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

		protected:
			const char* preset_to_desc(UIStylePreset a_preset) const;

		private:
			using data_type = std::array<std::pair<UIStylePreset, UIStylePresetSelectorWidgetStrings>, stl::underlying(UIStylePreset::Max)>;

			static data_type m_data;
		};
	}

}

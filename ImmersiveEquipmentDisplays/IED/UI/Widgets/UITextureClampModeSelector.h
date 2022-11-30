#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UITextureClampModeSelector :
			public virtual UILocalizationInterface
		{
		public:
			UITextureClampModeSelector(
				Localization::ILocalization& a_localization);

			bool DrawTextureClampModeSelector(
				Localization::StringID a_label,
				TextureAddressMode&    a_data);

			const char* texture_clamp_mode_to_desc(
				TextureAddressMode a_data) const;
		};
	}

}

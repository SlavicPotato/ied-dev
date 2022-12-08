#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UITextureClampModeSelector
		{
		public:

			static bool DrawTextureClampModeSelector(
				Localization::StringID a_label,
				TextureAddressMode&    a_data);

			static const char* texture_clamp_mode_to_desc(
				TextureAddressMode a_data) ;
		};
	}

}

#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEffectShaderDepthModeSelector
		{
		public:
			static bool DrawDepthModeSelector(
				Localization::StringID a_label,
				DepthStencilDepthMode& a_data);

			static const char* depth_mode_to_desc(
				DepthStencilDepthMode a_data);
		};
	}

}

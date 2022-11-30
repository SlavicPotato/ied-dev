#pragma once

#include "IED/UI/UILocalizationInterface.h"


namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEffectShaderDepthModeSelector :
			public virtual UILocalizationInterface
		{
		public:
			UIEffectShaderDepthModeSelector(
				Localization::ILocalization& a_localization);

			bool DrawDepthModeSelector(
				Localization::StringID a_label,
				DepthStencilDepthMode& a_data);

		protected:
			const char* depth_mode_to_desc(
				DepthStencilDepthMode a_data) const;

		};
	}

}

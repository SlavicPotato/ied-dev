#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include <skse64/NiProperties.h>

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

		protected:
			const char* texture_clamp_mode_to_desc(
				TextureAddressMode a_data) const;

		private:
			using data_type = std::array<
				std::pair<
					TextureAddressMode,
					const char*>,
				4>;

			static data_type m_data;
		};
	}

}

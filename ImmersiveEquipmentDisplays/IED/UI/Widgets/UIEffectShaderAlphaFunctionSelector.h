#pragma once

#include "IED/UI/UILocalizationInterface.h"

#include <skse64/NiProperties.h>

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEffectShaderAlphaFunctionSelector :
			public virtual UILocalizationInterface
		{
		public:
			UIEffectShaderAlphaFunctionSelector(
				Localization::ILocalization& a_localization);

			bool DrawAlphaFuncSelector(
				Localization::StringID     a_label,
				NiProperty::AlphaFunction& a_data);

		protected:
			const char* alpha_func_to_desc(
				NiProperty::AlphaFunction a_data) const;

		private:
			using data_type = std::array<
				std::pair<
					NiProperty::AlphaFunction,
					const char*>,
				11>;

			static data_type m_data;
		};
	}

}

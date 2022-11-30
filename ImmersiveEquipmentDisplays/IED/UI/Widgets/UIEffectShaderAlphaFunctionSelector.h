#pragma once

#include "IED/UI/UILocalizationInterface.h"


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

		};
	}

}

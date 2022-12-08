#pragma once

#include "IED/UI/UILocalizationInterface.h"


namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEffectShaderAlphaFunctionSelector
		{
		public:

			static bool DrawAlphaFuncSelector(
				Localization::StringID     a_label,
				NiProperty::AlphaFunction& a_data);

		protected:
			static const char* alpha_func_to_desc(
				NiProperty::AlphaFunction a_data);

		};
	}

}

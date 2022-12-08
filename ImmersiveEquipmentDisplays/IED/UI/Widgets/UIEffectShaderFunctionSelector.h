#pragma once

#include "IED/ConfigEffectShader.h"

#include "UIEffectShaderFunctionSelectorStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEffectShaderFunctionSelector
		{
		public:

			static bool DrawEffectShaderFunctionSelector(
				Data::EffectShaderFunctionType& a_type);

			static const char* esf_to_desc(Data::EffectShaderFunctionType a_type);
		};
	}

}

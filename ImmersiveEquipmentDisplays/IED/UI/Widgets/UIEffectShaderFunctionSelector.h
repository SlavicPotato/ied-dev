#pragma once

#include "IED/ConfigEffectShader.h"

#include "UIEffectShaderFunctionSelectorStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEffectShaderFunctionSelector :
			public virtual UILocalizationInterface
		{
		public:
			UIEffectShaderFunctionSelector(Localization::ILocalization& a_localization);

			bool DrawEffectShaderFunctionSelector(
				Data::EffectShaderFunctionType& a_type);

		protected:
			const char* esf_to_desc(Data::EffectShaderFunctionType a_type) const;

		private:
			using data_type = std::array<
				std::pair<
					Data::EffectShaderFunctionType,
					UIEffectShaderFunctionSelectorStrings>,
				2>;

			static data_type m_data;
		};
	}

}

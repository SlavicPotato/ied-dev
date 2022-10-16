#pragma once

#include "IED/ConditionalVariableStorage.h"

#include "IED/UI/UILocalizationInterface.h"

#include "UIVariableTypeSelectorWidgetStrings.h"

namespace IED
{
	namespace UI
	{
		class UIVariableTypeSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UIVariableTypeSelectorWidget(
				Localization::ILocalization& a_localization);

			bool DrawVariableTypeSelectorWidget(
				ConditionalVariableType& a_type);

		protected:
			const char* variable_type_to_desc(ConditionalVariableType a_type) const;

		private:
			using data_type = std::array<
				std::pair<
					ConditionalVariableType,
					UIVariableTypeSelectorWidgetStrings>,
				2>;

			static data_type m_data;
		};
	}

}
